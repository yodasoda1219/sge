/*
   Copyright 2022 Nora Beda and SGE contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include "sgmpch.h"
#include "panels/panels.h"
#include "editor_scene.h"
#include "texture_cache.h"
#include <sge/renderer/renderer.h>
#include <imgui_internal.h>
namespace sgm {
    editor_panel::editor_panel(const std::function<void(const std::string&)>& popup_callback) {
        m_popup_callback = popup_callback;
        m_callback_index = script_engine::add_on_reload_callback([this]() mutable {
            clear_section_header_cache();
        });
    }

    editor_panel::~editor_panel() { script_engine::remove_on_reload_callback(m_callback_index); }

    template <typename T, typename Func>
    static void draw_component(const std::string& name, entity e, const Func& callback) {
        if (e.has_all<T>()) {
            static constexpr ImGuiTreeNodeFlags tree_flags =
                ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_FramePadding;

            T& component = e.get_component<T>();
            ImVec2 region_available = ImGui::GetContentRegionAvail();
            ImGuiContext* imgui_context = ImGui::GetCurrentContext();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, 4.f));
            float line_height =
                imgui_context->Font->FontSize + imgui_context->Style.FramePadding.y * 2.f;

            bool open = ImGui::TreeNodeEx(name.c_str(), tree_flags);
            ImGui::PopStyleVar();

            ImGui::SameLine(region_available.x - line_height / 2.f);
            if (ImGui::Button("+", ImVec2(line_height, line_height))) {
                ImGui::OpenPopup("component-settings");
            }

            bool remove = false;
            if (ImGui::BeginPopup("component-settings")) {
                if (ImGui::MenuItem("Remove")) {
                    remove = true;
                }

                ImGui::EndPopup();
            }

            if (open) {
                callback(component);
                ImGui::TreePop();
            }

            if (remove) {
                e.remove_component<T>();
            }
        }
    }

    using component_add_callback = std::function<void(entity)>;

    template <typename T>
    static std::pair<std::string, component_add_callback> component_pair(const std::string& text) {
        component_add_callback callback = [](entity e) {
            if (!e.has_all<T>()) {
                e.add_component<T>();
            } else {
                spdlog::warn("attempted to add a component of type {0} to an entity that already "
                             "has the requested component!",
                             typeid(T).name());
            }
        };
        return std::make_pair(text, callback);
    }

    static const std::string collision_filter_editor_name = "Collision filter editor";
    void editor_panel::render() {
        entity& selection = editor_scene::get_selection();
        if (!selection) {
            ImGui::Text("No entity is selected.");
            return;
        }

        auto& tag = selection.get_component<tag_component>();
        ImGui::InputText("##entity-tag", &tag.tag);

        ImGui::SameLine();
        ImGui::PushItemWidth(-1.f);

        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("add-component");
        }

        if (ImGui::BeginPopup("add-component")) {
            // lets go unnecessary automation
            static std::unordered_map<std::string, component_add_callback> options = {
                component_pair<transform_component>("Transform"),
                component_pair<camera_component>("Camera"),
                component_pair<sprite_renderer_component>("Sprite renderer"),
                component_pair<rigid_body_component>("Rigid body"),
                component_pair<box_collider_component>("Box collider"),
                component_pair<script_component>("Script"),
            };

            for (const auto& [text, callback] : options) {
                if (ImGui::MenuItem(text.c_str())) {
                    callback(selection);

                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        draw_component<transform_component>(
            "Transform", selection, [selection](transform_component& transform) {
                ImGui::DragFloat2("Translation", &transform.translation.x, 0.25f);
                ImGui::DragFloat("Rotation", &transform.rotation);
                ImGui::DragFloat2("Scale", &transform.scale.x, 0.5f);

                if (ImGui::InputInt("Z layer", &transform.z_layer)) {
                    selection.get_scene()->recalculate_render_order();
                }
            });

        draw_component<camera_component>("Camera", selection, [](camera_component& camera) {
            static const std::vector<const char*> type_names = { "Orthographic", "Perspective" };
            ImGui::Checkbox("Primary", &camera.primary);

            int camera_type = (int)camera.camera.get_projection_type();
            if (ImGui::Combo("Camera type", &camera_type, type_names.data(), type_names.size())) {
                camera.camera.set_projection_type((projection_type)camera_type);
            }

            if (camera.camera.get_projection_type() == projection_type::orthographic) {
                float view_size = camera.camera.get_orthographic_size();
                if (ImGui::DragFloat("View size", &view_size)) {
                    camera.camera.set_orthographic_size(view_size);
                }

                float near_clip = camera.camera.get_orthographic_near_plane();
                if (ImGui::DragFloat("Near clip", &near_clip, 0.01f)) {
                    camera.camera.set_orthographic_near_plane(near_clip);
                }

                float far_clip = camera.camera.get_orthographic_far_plane();
                if (ImGui::DragFloat("Far clip", &far_clip, 0.01f)) {
                    camera.camera.set_orthographic_far_plane(far_clip);
                }
            } else {
                float fov = camera.camera.get_vertical_fov();
                if (ImGui::SliderFloat("Vertical field of view", &fov, 1.f, 89.f)) {
                    camera.camera.set_vertical_fov(fov);
                }

                float near_clip = camera.camera.get_perspective_near_plane();
                if (ImGui::DragFloat("Near clip", &near_clip)) {
                    camera.camera.set_perspective_near_plane(near_clip);
                }

                float far_clip = camera.camera.get_perspective_far_plane();
                if (ImGui::DragFloat("Far clip", &far_clip)) {
                    camera.camera.set_perspective_far_plane(far_clip);
                }
            }
        });

        draw_component<sprite_renderer_component>(
            "Sprite renderer", selection, [this, selection](sprite_renderer_component& component) {
                ImGui::ColorEdit4("Color", &component.color.x);


                ref<asset> _asset = component.texture;
                if (ImGui::InputAsset("Texture", &_asset, "texture", "texture_2d")) {
                    component.texture = _asset.as<texture_2d>();
                }

                static const std::string shader_name = "shader";
                _asset = component._shader;
                if (ImGui::InputAsset("Shader", &_asset, shader_name, shader_name)) {
                    component._shader = _asset.as<shader>();
                    selection.get_scene()->recalculate_render_order();
                }
            });

        draw_component<
            rigid_body_component>("Rigid body", selection, [this](rigid_body_component& component) {
            static const std::vector<const char*> type_names = { "Static", "Kinematic", "Dynamic" };
            int32_t type_index = (int32_t)component.type;
            if (ImGui::Combo("Body type", &type_index, type_names.data(), type_names.size())) {
                component.type = (rigid_body_component::body_type)type_index;
            }

            ImGui::Checkbox("Fixed rotation", &component.fixed_rotation);

            if (ImGui::CollapsingHeader("Collision filter")) {
                ImGui::Indent();
                bool editor_open = m_popup_manager->is_open(collision_filter_editor_name);

                ImGui::Text("Category");
                ImGui::SameLine();

                if (ImGui::Button("Edit##category-edit-button") && !editor_open) {
                    m_filter_editor_data.type = filter_editor_type::category;
                    m_filter_editor_data.field = &component.filter_category;

                    m_popup_manager->open(collision_filter_editor_name);
                }

                ImGui::Text("Mask");
                ImGui::SameLine();

                if (ImGui::Button("Edit##mask-edit-button") && !editor_open) {
                    m_filter_editor_data.type = filter_editor_type::mask;
                    m_filter_editor_data.field = &component.filter_mask;

                    m_popup_manager->open(collision_filter_editor_name);
                }

                ImGui::Unindent();
            }
        });

        draw_component<box_collider_component>(
            "Box collider", selection, [this](box_collider_component& component) {
                ImGui::DragFloat("Density", &component.density, 0.1f);
                ImGui::DragFloat("Friction", &component.friction, 0.1f);
                ImGui::DragFloat("Restitution", &component.restitution, 0.1f);
                ImGui::DragFloat("Restitution threashold", &component.restitution_threashold, 0.1f);
                ImGui::DragFloat2("Size", &component.size.x, 0.01f);
            });

        draw_component<script_component>(
            "Script", selection, [this, selection](script_component& component) {
                // todo: script cache
                std::optional<size_t> assembly_index = project::get().get_assembly_index();
                if (!assembly_index.has_value()) {
                    ImGui::TextColored(ImVec4(0.9, 0.f, 0.f, 1.f),
                                       "The project script assembly failed compilation.");
                    return;
                }

                void* assembly = script_engine::get_assembly(assembly_index.value());
                auto is_class_valid = [assembly](const std::string& name) mutable {
                    void* _class = script_engine::get_class(assembly, name);
                    return _class != nullptr;
                };

                bool valid_script = is_class_valid(component.class_name);
                bool invalid_name = !valid_script;

                if (invalid_name) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.f, 0.f, 1.f));
                }

                if (ImGui::InputText("Script Name", &component.class_name)) {
                    valid_script = is_class_valid(component.class_name);

                    auto _scene = editor_scene::get_scene();
                    if (valid_script) {
                        void* _class = script_engine::get_class(assembly, component.class_name);
                        _scene->set_script(selection, _class);
                    } else {
                        _scene->set_script(selection, nullptr);
                    }
                }

                if (invalid_name) {
                    ImGui::PopStyleColor();
                    component.enabled = true;
                } else {
                    ImGui::Checkbox("Enabled", &component.enabled);
                }

                if (valid_script) {
                    draw_property_controls();
                }
            });
    }

    void editor_panel::register_popups(popup_manager& popup_manager_) {
        {
            popup_manager::popup_data data;
            data.callback = [this]() mutable {
                std::string text;
                switch (m_filter_editor_data.type) {
                case filter_editor_type::category:
                    text = "Select the collision categories in which this object resides.";
                    break;
                case filter_editor_type::mask:
                    text = "Select the categories of objects this object can collide with.";
                    break;
                }

                ImGui::Text("%s", text.c_str());
                ImGuiStyle& style = ImGui::GetStyle();

                auto _scene = editor_scene::get_scene();
                uint16_t& field = *m_filter_editor_data.field;

                for (size_t i = 0; i < scene::collision_category_count; i++) {
                    uint16_t bit = 0x1 << (uint16_t)i;

                    std::string section_id = "toggle-" + std::to_string(i);
                    ImGui::PushID(section_id.c_str());

                    ImGui::Columns(3, nullptr, false);
                    ImGui::SetColumnWidth(0, 30.f);
                    ImGui::SetColumnWidth(1, 100.f);

                    bool enabled = (field & bit) != 0;
                    if (ImGui::Checkbox("##checkbox", &enabled)) {
                        if (enabled) {
                            field |= bit;
                        } else {
                            field &= ~bit;
                        }
                    }

                    std::string& category_label = _scene->collision_category_name(i);
                    bool category_label_exists = category_label.length() > 0;

                    std::string default_label = "Category " + std::to_string(i + 1);
                    ImGui::NextColumn();
                    ImGui::PushItemWidth(-1.f);

                    std::string button_text;
                    if (category_label_exists) {
                        ImGui::InputText("##set-label", &category_label);

                        button_text = "x";
                    } else {
                        std::string label = category_label_exists ? category_label : default_label;
                        ImGui::Text("%s", label.c_str());

                        button_text = "-";
                    }

                    ImGui::PopItemWidth();
                    ImGui::NextColumn();

                    if (ImGui::Button(button_text.c_str())) {
                        if (category_label_exists) {
                            category_label.clear();
                        } else {
                            category_label = default_label;
                        }
                    }

                    ImGui::Columns();
                    ImGui::PopID();
                }

                ImGui::Separator();
                if (ImGui::Button("Close")) {
                    ImGui::CloseCurrentPopup();
                }
            };

            popup_manager_.register_popup(collision_filter_editor_name, data);
        }

        m_popup_manager = &popup_manager_;
    }

    void editor_panel::cache_script_class(void* _class) {
        if (m_section_header_cache.find(_class) != m_section_header_cache.end()) {
            return;
        }

        std::vector<void*> properties;
        script_engine::iterate_properties(_class, properties);

        void* attribute_type = script_helpers::get_core_type("SGE.SectionHeaderAttribute", true);
        void* header_names_property = script_engine::get_property(attribute_type, "HeaderNames");

        std::vector<section_header_t> headers;
        for (void* property : properties) {
            if (!script_helpers::is_property_serializable(property)) {
                continue;
            }

            std::vector<std::string> header_names;
            if (script_helpers::property_has_attribute(property, attribute_type)) {
                auto attribute = script_helpers::get_property_attribute(property, attribute_type);
                void* object = attribute->get();

                void* list = script_engine::get_property_value(object, header_names_property);
                void* list_type = script_engine::get_class_from_object(list);

                void* count_property = script_engine::get_property(list_type, "Count");
                void* returned = script_engine::get_property_value(list, count_property);
                int32_t count = script_engine::unbox_object<int32_t>(returned);

                void* item_property = script_engine::get_property(list_type, "Item");
                for (int32_t i = 0; i < count; i++) {
                    returned = script_engine::get_property_value(list, item_property, &i);

                    std::string name = script_engine::from_managed_string(returned);
                    header_names.push_back(name);
                }
            } else {
                header_names.push_back("");
            }

            section_header_t* current_node = nullptr;
            for (const auto& name : header_names) {
                std::vector<section_header_t>* nodes;
                if (current_node != nullptr) {
                    nodes = &current_node->subheaders;
                } else {
                    nodes = &headers;
                }

                section_header_t* found = nullptr;
                for (auto& current : *nodes) {
                    if (current.name == name) {
                        found = &current;
                    }
                }

                if (found != nullptr) {
                    current_node = found;
                } else {
                    current_node = &nodes->emplace_back();
                    current_node->name = name;
                }
            }

            if (current_node != nullptr) {
                current_node->properties.push_back(property);
            }
        }

        m_section_header_cache.insert(std::make_pair(_class, headers));
    }

    void editor_panel::draw_property_controls() {
        const auto& selection = editor_scene::get_selection();
        auto _scene = editor_scene::get_scene();
        _scene->verify_script(selection);

        auto& sc = selection.get_component<script_component>();
        void* instance = sc.instance->get();

        if (m_section_header_cache.find(sc._class) == m_section_header_cache.end()) {
            cache_script_class(sc._class);
        }

        const auto& headers = m_section_header_cache[sc._class];
        for (const auto& header : headers) {
            render_header(instance, header);
        }
    }

    void editor_panel::render_header(void* script_object, const section_header_t& header) {
        bool indented = false;
        if (!header.name.empty()) {
            if (!ImGui::CollapsingHeader(header.name.c_str())) {
                return;
            }

            ImGui::Indent();
            indented = true;
        }

        for (void* property : header.properties) {
            std::string property_name = script_engine::get_property_name(property);
            // todo: format label?

            script_helpers::show_property_control(script_object, property, property_name);
        }

        for (const auto& subheader : header.subheaders) {
            render_header(script_object, subheader);
        }

        if (indented) {
            ImGui::Unindent();
        }
    }
} // namespace sgm

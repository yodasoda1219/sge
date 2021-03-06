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

#pragma once

#include "sge/events/event.h"
#include "sge/events/window_events.h"
#include "sge/scene/editor_camera.h"
#include "sge/core/guid.h"
#include <entt/entt.hpp>

namespace sge {

    class entity;
    class scene_serializer;
    struct script_deserializer;
    class scene_contact_listener;
    struct scene_physics_data;

    // A Scene is a set of entities and components.
    class scene : public ref_counted {
    public:
        static constexpr size_t collision_category_count = sizeof(uint16_t) * 8;

        scene() = default;
        ~scene();
        scene(const scene&) = delete;
        scene& operator=(const scene&) = delete;

        entity create_entity(const std::string& name = std::string());
        entity create_entity(guid id, const std::string& name = std::string());
        entity clone_entity(entity src, const std::string& name = std::string());

        void destroy_entity(entity entity);
        void clear();

        void set_script(entity e, void* _class);
        void reset_script(entity e);
        void verify_script(entity e);

        void update_physics_data(entity e);
        void recalculate_render_order();

        bool apply_force(entity e, glm::vec2 force, glm::vec2 point, bool wake = true);
        bool apply_force(entity e, glm::vec2 force, bool wake = true);
        bool apply_linear_impulse(entity e, glm::vec2 impulse, glm::vec2 point, bool wake = true);
        bool apply_linear_impulse(entity e, glm::vec2 impulse, bool wake = true);
        bool apply_torque(entity e, float torque, bool wake = true);

        std::optional<glm::vec2> get_velocity(entity e);
        bool set_velocity(entity e, glm::vec2 velocity);

        std::optional<float> get_angular_velocity(entity e);
        bool set_angular_velocity(entity e, float velocity);

        entity find_guid(guid id);
        ref<scene> copy();

        std::string& collision_category_name(size_t index) {
            return m_collision_category_names[index];
        }

        void on_start();
        void on_stop();
        void on_runtime_update(timestep ts);
        void on_editor_update(timestep ts, const editor_camera& camera);
        void on_event(event& e);

        void set_viewport_size(uint32_t width, uint32_t height);

        void for_each(const std::function<void(entity)>& callback);

        template <typename... T>
        void for_each(const std::function<void(entity)>& callback) {
            auto view = m_registry.view<T...>();
            for (entt::entity id : view) {
                view_iteration(id, callback);
            }
        }

    private:
        template <typename T>
        void on_component_added(const entity& e, T& component) {
            // no behavior
        }

        template <typename T>
        void on_component_removed(const entity& e, T& component) {
            // no behavior
        }

        void view_iteration(entt::entity id, const std::function<void(entity)>& callback);
        void render();

        void remove_script(entity e, void* component = nullptr);
        guid get_guid(entity e);

        entt::registry m_registry;
        std::vector<entity> m_render_order;
        uint32_t m_viewport_width, m_viewport_height;

        scene_physics_data* m_physics_data = nullptr;
        std::array<std::string, collision_category_count> m_collision_category_names;

        friend class entity;
        friend class scene_contact_listener;
        friend class scene_serializer;
        friend struct script_deserializer;

        template <typename T>
        friend struct scene_component_copier;
    };
} // namespace sge

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
#include <sge/imgui/popup_manager.h>
namespace sge {
    enum class panel_id { renderer_info, viewport, scene_hierarchy, editor, content_browser };

    class panel {
    public:
        virtual ~panel() = default;

        virtual void update(timestep ts) {}
        virtual void on_event(event& e) {}

        virtual void register_popups(popup_manager& popup_manager_) { }
        virtual void begin(const char* title, bool* open) { ImGui::Begin(title, open); }
        virtual void render() = 0;

        virtual std::string get_title() = 0;
        virtual panel_id get_id() = 0;

        bool& open() { return m_open; }

    protected:
        bool m_open = true;
    };
} // namespace sge

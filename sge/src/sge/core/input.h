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
#include "sge/core/key_codes.h"
#include "sge/events/event.h"
namespace sge {
    class input {
    public:
        input() = delete;

        static void init();
        static void shutdown();
        static void on_event(event& e);

        static bool get_key(key_code code);
        static bool get_mouse_button(mouse_button button);

        static glm::vec2 get_mouse_position();
        static void set_mouse_position(glm::vec2 position);

        // todo: action mapping
    };
} // namespace sge
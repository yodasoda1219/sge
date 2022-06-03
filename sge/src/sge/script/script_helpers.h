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
#include "sge/scene/scene.h"
#include "sge/scene/entity.h"
namespace sge {
    class script_helpers {
    public:
        script_helpers() = delete;

        static void init(void* helpers_class);
        static void report_exception(void* exception);

        static bool property_has_attribute(void* property, void* attribute_type);
        static uint32_t get_property_attribute(void* property, void* attribute_type);
        static bool is_property_serializable(void* property);

        static void* create_entity_object(entity e);
        static entity get_entity_from_object(void* object);

        static void* get_core_type(const std::string& name, bool scriptcore = false);

        static std::string get_type_name_safe(void* _class);
        static size_t get_type_size(void* _class);
        static bool type_is_array(void* _class);
        
        static void* create_event_object(event& e);
        static void* create_list_object(void* element_type);
    };
} // namespace sge
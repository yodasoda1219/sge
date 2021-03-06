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
#include "sge/core/environment.h"

namespace sge {
    int32_t windows_run_command(const process_info& info);

    bool windows_setenv(const std::string& key, const std::string& value);
    std::string windows_getenv(const std::string& key);
    bool windows_hasenv(const std::string& key);

    fs::path windows_get_home_directory();
    uint64_t windows_get_process_id();
}
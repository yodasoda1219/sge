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
#include "sge/imgui/imgui_backend.h"
namespace sge {
    class desktop_imgui_backend : public imgui_backend {
    public:
        desktop_imgui_backend();
        virtual ~desktop_imgui_backend() override;

        virtual void begin() override;
    };
} // namespace sge
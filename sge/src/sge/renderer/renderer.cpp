/*
   Copyright 2021 Nora Beda

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

#include "sgepch.h"
#include "sge/renderer/renderer.h"
#ifdef SGE_USE_VULKAN
#include "sge/platform/vulkan/vulkan_renderer.h"
#endif
namespace sge {
    static std::unique_ptr<renderer_api> renderer_api_;

    void renderer::init() {
        {
            renderer_api* api_instance = nullptr;

#ifdef SGE_USE_VULKAN
            if (api_instance == nullptr) {
                api_instance = new vulkan_renderer;
            }
#endif

            renderer_api_ = std::unique_ptr<renderer_api>(api_instance);
        }

        renderer_api_->init();
    }

    void renderer::shutdown() {
        renderer_api_->shutdown();
        renderer_api_.reset();
    }
}
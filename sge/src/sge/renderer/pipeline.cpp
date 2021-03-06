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

#include "sgepch.h"
#include "sge/renderer/pipeline.h"
#ifdef SGE_USE_VULKAN
#include "sge/platform/vulkan/vulkan_base.h"
#include "sge/platform/vulkan/vulkan_pipeline.h"
#endif
namespace sge {
    ref<pipeline> pipeline::create(const pipeline_spec& spec) {
#ifdef SGE_USE_VULKAN
        return ref<vulkan_pipeline>::create(spec);
#endif

        return nullptr;
    }
} // namespace sge
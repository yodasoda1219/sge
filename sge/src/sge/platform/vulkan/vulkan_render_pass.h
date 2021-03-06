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
#include "sge/renderer/render_pass.h"
#include "sge/platform/vulkan/vulkan_swapchain.h"
#include "sge/platform/vulkan/vulkan_framebuffer.h"
namespace sge {
    class vulkan_render_pass : public render_pass {
    public:
        vulkan_render_pass(vulkan_swapchain* parent);
        vulkan_render_pass(vulkan_framebuffer* parent);
        virtual ~vulkan_render_pass() override;

        virtual render_pass_parent_type get_parent_type() override;

        virtual void begin(command_list& cmdlist, const glm::vec4& clear_color) override;
        virtual void end(command_list& cmdlist) override;

        VkRenderPass get() { return m_render_pass; }
        vulkan_swapchain* get_swapchain_parent() { return m_swapchain_parent; }
        vulkan_framebuffer* get_framebuffer_parent() { return m_framebuffer_parent; }

    private:
        vulkan_swapchain* m_swapchain_parent = nullptr;
        vulkan_framebuffer* m_framebuffer_parent = nullptr;
        VkRenderPass m_render_pass;
    };
} // namespace sge
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
#include "sge/renderer/renderer.h"
#include "sge/platform/vulkan/vulkan_base.h"
#include "sge/platform/vulkan/vulkan_image.h"
#include "sge/platform/vulkan/vulkan_allocator.h"
#include "sge/platform/vulkan/vulkan_command_list.h"
#include "sge/platform/vulkan/vulkan_context.h"
#include "sge/platform/vulkan/vulkan_buffer.h"
#include "sge/platform/vulkan/vulkan_texture.h"

namespace sge {
    VkFormat get_vulkan_image_format(image_format format) {
        switch (format) {
        case image_format::RGB8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case image_format::RGB8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case image_format::RGBA8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case image_format::RGBA8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        default:
            throw std::runtime_error("invalid image format!");
            return VK_FORMAT_MAX_ENUM;
        }
    }

    VkImageUsageFlags get_vulkan_image_usage(uint32_t usage) {
        if (usage == image_usage_none) {
            throw std::runtime_error("must provide usage flags!");
        }

        VkImageUsageFlags flags = 0;
        if ((usage & image_usage_texture) != 0) {
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if ((usage & image_usage_attachment) != 0) {
            // maybe pass depth boolean?
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if ((usage & image_usage_storage) != 0) {
            flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        }

        if ((usage & image_usage_transfer) != 0) {
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        return flags;
    }

    vulkan_image_2d::vulkan_image_2d(const image_spec& spec) {
        m_spec = spec;
        m_format = get_vulkan_image_format(m_spec.format);
        m_usage = get_vulkan_image_usage(m_spec.image_usage);

        // todo(nora): change if/when depth?
        m_aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

        create_image();
        create_view();
    }

    vulkan_image_2d::~vulkan_image_2d() {
        VkDevice device = vulkan_context::get().get_device().get();
        vkDestroyImageView(device, m_view, nullptr);

        vulkan_allocator::free(m_image, m_allocation);
    }

    // shamelessly stolen from my other project
    // https://github.com/yodasoda1219/vkrollercoaster/blob/main/src/image.cpp
    static void get_stage_and_mask(VkImageLayout layout, VkPipelineStageFlags& stage,
                                   VkAccessFlags& access_mask) {
        switch (layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED:
            stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            access_mask = 0;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            access_mask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            access_mask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            access_mask = VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_GENERAL:
            stage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            access_mask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            stage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            access_mask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            stage = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
            access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
            break;
        default:
            throw std::runtime_error("unimplemented/unsupported image layout!");
        }
    }

    void vulkan_image_2d::set_layout(VkImageLayout new_layout, command_list* cmdlist) {
        auto barrier = vk_init<VkImageMemoryBarrier>(VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER);
        barrier.image = m_image;
        barrier.oldLayout = m_layout;
        barrier.newLayout = new_layout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.subresourceRange.aspectMask = m_aspect;

        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = m_spec.mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = m_spec.array_layers;

        VkPipelineStageFlags source_stage, destination_stage;
        get_stage_and_mask(m_layout, source_stage, barrier.srcAccessMask);
        get_stage_and_mask(new_layout, destination_stage, barrier.dstAccessMask);

        vulkan_command_list* vk_cmdlist;
        if (cmdlist != nullptr) {
            vk_cmdlist = (vulkan_command_list*)cmdlist;
        } else {
            auto transfer_queue = renderer::get_queue(command_list_type::transfer);

            auto& transfer_cmdlist = transfer_queue->get();
            transfer_cmdlist.begin();
            vk_cmdlist = (vulkan_command_list*)&transfer_cmdlist;
        }

        VkCommandBuffer cmdbuffer = vk_cmdlist->get();
        vkCmdPipelineBarrier(cmdbuffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr,
                             1, &barrier);

        if (cmdlist == nullptr) {
            auto transfer_queue = renderer::get_queue(command_list_type::transfer);

            vk_cmdlist->end();
            transfer_queue->submit(*vk_cmdlist, true);
        }

        m_layout = new_layout;
        for (auto tex : m_dependents) {
            tex->on_layout_transition();
        }
    }

    void vulkan_image_2d::copy_from(const void* data, size_t size) {
        auto staging_buffer = ref<vulkan_buffer>::create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                         VMA_MEMORY_USAGE_CPU_TO_GPU);
        staging_buffer->map();
        memcpy(staging_buffer->mapped, data, size);
        staging_buffer->unmap();

        copy_from(staging_buffer);
    }

    bool vulkan_image_2d::copy_to(void* data, size_t size) {
        if ((m_usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == 0) {
            return false;
        }

        auto queue = renderer::get_queue(command_list_type::transfer);
        auto buffer = ref<vulkan_buffer>::create(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                 VMA_MEMORY_USAGE_GPU_TO_CPU);

        auto& cmdlist = (vulkan_command_list&)queue->get();
        cmdlist.begin();

        VkImageLayout original_layout = m_layout;
        set_layout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, &cmdlist);

        auto region = vk_init<VkBufferImageCopy>();
        region.imageSubresource.aspectMask = m_aspect;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageSubresource.mipLevel = 0;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent.width = m_spec.width;
        region.imageExtent.height = m_spec.height;
        region.imageExtent.depth = 1;

        auto cmdbuffer = cmdlist.get();
        vkCmdCopyImageToBuffer(cmdbuffer, m_image, m_layout, buffer->get(), 1, &region);
        set_layout(original_layout, &cmdlist);

        cmdlist.end();
        queue->submit(cmdlist, true);

        buffer->map();
        memcpy(data, buffer->mapped, size);
        buffer->unmap();

        return true;
    }

    void vulkan_image_2d::create_image() {
        auto create_info = vk_init<VkImageCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO);
        create_info.imageType = VK_IMAGE_TYPE_2D;

        create_info.extent.width = m_spec.width;
        create_info.extent.height = m_spec.height;
        create_info.extent.depth = 1;

        create_info.mipLevels = m_spec.mip_levels;
        create_info.arrayLayers = m_spec.array_layers;

        create_info.format = m_format;
        create_info.tiling = VK_IMAGE_TILING_OPTIMAL;

        create_info.initialLayout = m_layout;
        create_info.usage = m_usage;
        create_info.samples = VK_SAMPLE_COUNT_1_BIT;

        vulkan_device& device = vulkan_context::get().get_device();
        auto physical_device = device.get_physical_device();

        vulkan_physical_device::queue_family_indices indices;
        physical_device.query_queue_families(
            VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, indices);

        std::set<uint32_t> index_set = { indices.graphics.value(), indices.compute.value(),
                                         indices.transfer.value() };
        std::vector<uint32_t> queue_families(index_set.begin(), index_set.end());

        if (queue_families.size() > 1) {
            create_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.pQueueFamilyIndices = queue_families.data();
            create_info.queueFamilyIndexCount = queue_families.size();
        } else {
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        auto alloc_info = vk_init<VmaAllocationCreateInfo>();
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vulkan_allocator::alloc(create_info, alloc_info, m_image, m_allocation);
    }

    void vulkan_image_2d::create_view() {
        auto create_info = vk_init<VkImageViewCreateInfo>(VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
        create_info.image = m_image;
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = m_format;

        create_info.subresourceRange.aspectMask = m_aspect;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = m_spec.array_layers;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = m_spec.mip_levels;

        VkDevice device = vulkan_context::get().get_device().get();
        VkResult result = vkCreateImageView(device, &create_info, nullptr, &m_view);
        check_vk_result(result);
    }

    void vulkan_image_2d::copy_from(ref<vulkan_buffer> source) {
        auto transfer_queue = renderer::get_queue(command_list_type::transfer);
        auto& cmdlist = (vulkan_command_list&)transfer_queue->get();
        cmdlist.begin();

        if (m_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
            set_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, &cmdlist);
        }

        VkImageLayout original_layout = m_layout;
        set_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &cmdlist);

        VkCommandBuffer cmdbuffer = cmdlist.get();
        if (m_spec.mip_levels == 1) {
            auto region = vk_init<VkBufferImageCopy>();

            region.imageSubresource.aspectMask = m_aspect;
            region.imageSubresource.mipLevel = 0;
            region.imageSubresource.baseArrayLayer = 0;
            region.imageSubresource.layerCount = m_spec.array_layers;

            region.imageExtent.width = m_spec.width;
            region.imageExtent.height = m_spec.height;
            region.imageExtent.depth = 1;

            vkCmdCopyBufferToImage(cmdbuffer, source->get(), m_image, m_layout, 1, &region);
        } else {
            throw std::runtime_error("can't copy more than one mip level yet");
        }

        set_layout(original_layout, &cmdlist);

        cmdlist.end();
        transfer_queue->submit(cmdlist, true);
    }
} // namespace sge
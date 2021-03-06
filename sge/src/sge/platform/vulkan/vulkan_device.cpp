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
#include "sge/platform/vulkan/vulkan_base.h"
#include "sge/platform/vulkan/vulkan_device.h"
#include "sge/platform/vulkan/vulkan_context.h"
#if defined(SGE_AFTERMATH_LINKED) && defined(SGE_DEBUG)
#include <NsightAftermathGpuCrashTracker.h>
#define SGE_USE_AFTERMATH
#endif
namespace sge {
    std::vector<vulkan_physical_device> vulkan_physical_device::enumerate() {
        VkInstance instance = vulkan_context::get().get_instance();
        std::vector<vulkan_physical_device> devices;

        uint32_t physical_device_count = 0;
        vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
        if (physical_device_count == 0) {
            return devices;
        }

        std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
        vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

        for (auto physical_device : physical_devices) {
            devices.push_back(vulkan_physical_device(physical_device));
        }
        return devices;
    }

    bool vulkan_physical_device::is_extension_supported(const std::string& name) const {
        uint32_t extension_count = 0;
        vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extension_count, nullptr);
        if (extension_count == 0) {
            return false;
        }

        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(m_device, nullptr, &extension_count,
                                             extensions.data());

        for (const auto& extension : extensions) {
            if (name == extension.extensionName) {
                return true;
            }
        }

        return false;
    }

    void vulkan_physical_device::query_queue_families(VkQueueFlags query,
                                                      queue_family_indices& indices) const {
        uint32_t family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_device, &family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_device, &family_count, queue_families.data());

        for (uint32_t i = 0; i < family_count; i++) {
            VkQueueFlags found = 0;
            const auto& queue_family = queue_families[i];

            auto query_family = [&](std::optional<uint32_t>& index, VkQueueFlagBits queue) {
                if (query & queue) {
                    if ((!index.has_value() ||
                         (queue_families[index.value()].queueFlags & queue) == 0) &&
                        (queue_family.queueFlags & queue)) {
                        index = i;
                    }

                    if (index.has_value()) {
                        found |= queue;
                    }
                }
            };

            query_family(indices.graphics, VK_QUEUE_GRAPHICS_BIT);
            query_family(indices.compute, VK_QUEUE_COMPUTE_BIT);
            query_family(indices.transfer, VK_QUEUE_TRANSFER_BIT);

            if (found == query) {
                break;
            }
        }
    }

    void vulkan_physical_device::get_properties(VkPhysicalDeviceProperties& properties) const {
        vkGetPhysicalDeviceProperties(m_device, &properties);
    }

    void vulkan_physical_device::get_features(VkPhysicalDeviceFeatures& features) const {
        vkGetPhysicalDeviceFeatures(m_device, &features);
    }

    void vulkan_physical_device::get_surface_capabilities(
        VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities) const {
        VkInstance instance = vulkan_context::get().get_instance();
        auto fpGetPhysicalDeviceSurfaceCapabilitiesKHR =
            (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(
                instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        if (fpGetPhysicalDeviceSurfaceCapabilitiesKHR == nullptr) {
            throw std::runtime_error("could not find vkGetPhysicalDeviceSurfaceCapabilitiesKHR!");
        }

        fpGetPhysicalDeviceSurfaceCapabilitiesKHR(m_device, surface, &capabilities);
    }

    void vulkan_physical_device::get_surface_formats(
        VkSurfaceKHR surface, std::vector<VkSurfaceFormatKHR>& formats) const {
        VkInstance instance = vulkan_context::get().get_instance();
        auto fpGetPhysicalDeviceSurfaceFormatsKHR =
            (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(
                instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        if (fpGetPhysicalDeviceSurfaceFormatsKHR == nullptr) {
            throw std::runtime_error("could not find vkGetPhysicalDeviceSurfaceFormatsKHR!");
        }

        uint32_t format_count = 0;
        fpGetPhysicalDeviceSurfaceFormatsKHR(m_device, surface, &format_count, nullptr);

        if (format_count > 0) {
            formats.resize(format_count);
            fpGetPhysicalDeviceSurfaceFormatsKHR(m_device, surface, &format_count, formats.data());
        } else {
            formats.clear();
        }
    }

    void vulkan_physical_device::get_surface_present_modes(
        VkSurfaceKHR surface, std::vector<VkPresentModeKHR>& present_modes) const {
        VkInstance instance = vulkan_context::get().get_instance();
        auto fpGetPhysicalDeviceSurfacePresentModesKHR =
            (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(
                instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
        if (fpGetPhysicalDeviceSurfacePresentModesKHR == nullptr) {
            throw std::runtime_error("could not find vkGetPhysicalDeviceSurfacePresentModesKHR!");
        }

        uint32_t present_mode_count = 0;
        fpGetPhysicalDeviceSurfacePresentModesKHR(m_device, surface, &present_mode_count, nullptr);

        if (present_mode_count > 0) {
            present_modes.resize(present_mode_count);
            fpGetPhysicalDeviceSurfacePresentModesKHR(m_device, surface, &present_mode_count,
                                                      present_modes.data());
        } else {
            present_modes.clear();
        }
    }

    std::optional<uint32_t> vulkan_physical_device::find_surface_present_queue(
        VkSurfaceKHR surface) const {
        VkInstance instance = vulkan_context::get().get_instance();
        auto fpGetPhysicalDeviceSurfaceSupportKHR =
            (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(
                instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        if (fpGetPhysicalDeviceSurfaceSupportKHR == nullptr) {
            throw std::runtime_error("could not find vkGetPhysicalDeviceSurfaceSupportKHR!");
        }

        uint32_t family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_device, &family_count, nullptr);

        std::optional<uint32_t> family;
        for (uint32_t i = 0; i < family_count; i++) {
            VkBool32 presentation_supported = false;
            fpGetPhysicalDeviceSurfaceSupportKHR(m_device, i, surface, &presentation_supported);

            if (presentation_supported) {
                family = i;
                break;
            }
        }

        return family;
    }

    vulkan_device::vulkan_device(const vulkan_physical_device& physical_device) {
        m_crash_tracker = nullptr;
        m_physical_device = physical_device;
        create();
    }

    vulkan_device::~vulkan_device() {
        vkDeviceWaitIdle(m_device);
        vkDestroyDevice(m_device, nullptr);

#ifdef SGE_USE_AFTERMATH
        if (m_crash_tracker != nullptr) {
            delete (GpuCrashTracker*)m_crash_tracker;
        }
#endif
    }

    VkQueue vulkan_device::get_queue(uint32_t family) {
        VkQueue queue;
        vkGetDeviceQueue(m_device, family, 0, &queue);
        return queue;
    }

    void vulkan_device::create() {
        auto& context = vulkan_context::get();
        VkPhysicalDevice physical_device = m_physical_device.get();

        auto selected_extensions = context.get_device_extensions();
        auto verify_extension = [&](const std::string& name) {
            if (selected_extensions.find(name) != selected_extensions.end()) {
                return true;
            }

            bool supported = m_physical_device.is_extension_supported(name);
            if (supported) {
                selected_extensions.insert(name);
            }

            return supported;
        };

        // if VK_KHR_portability_subset is available, add it
        // https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VK_KHR_portability_subset.html
        verify_extension("VK_KHR_portability_subset");

        using aftermath_info_t = VkDeviceDiagnosticsConfigCreateInfoNV;
        std::unique_ptr<aftermath_info_t> aftermath_info;
#ifdef SGE_USE_AFTERMATH
        if (verify_extension(VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME) &&
            verify_extension(VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME)) {
            spdlog::info("enabling GPU crash dumping");

            auto crash_tracker = new GpuCrashTracker;
            crash_tracker->Initialize();
            m_crash_tracker = crash_tracker;

            aftermath_info = std::make_unique<aftermath_info_t>(vk_init<aftermath_info_t>(
                VK_STRUCTURE_TYPE_DEVICE_DIAGNOSTICS_CONFIG_CREATE_INFO_NV));
            aftermath_info->flags =
                (VkDeviceDiagnosticsConfigFlagBitsNV)(VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_RESOURCE_TRACKING_BIT_NV |
                                                      VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_AUTOMATIC_CHECKPOINTS_BIT_NV |
                                                      VK_DEVICE_DIAGNOSTICS_CONFIG_ENABLE_SHADER_DEBUG_INFO_BIT_NV);
        }
#endif

        // convert to vector
        std::vector<const char*> device_extensions;
        for (const auto& selected_extension : selected_extensions) {
            if (m_physical_device.is_extension_supported(selected_extension)) {
                device_extensions.push_back(selected_extension.c_str());
            } else {
                spdlog::warn("device extension {0} is not present", selected_extension);
            }
        }

        const auto& layer_names = context.get_device_layers();
        std::vector<const char*> device_layers;
        {
            uint32_t layer_count = 0;
            vkEnumerateDeviceLayerProperties(physical_device, &layer_count, nullptr);
            std::vector<VkLayerProperties> layers(layer_count);
            vkEnumerateDeviceLayerProperties(physical_device, &layer_count, layers.data());

            // verify layer availability
            for (const auto& selected_layer : layer_names) {
                bool found = false;
                for (const auto& layer_data : layers) {
                    if (selected_layer == layer_data.layerName) {
                        found = true;
                        break;
                    }
                }
                if (found) {
                    device_layers.push_back(selected_layer.c_str());
                } else {
                    spdlog::warn("device layer {0} is not present", selected_layer);
                }
            }
        }

        static const float queue_priority = 1.f;
        std::vector<VkDeviceQueueCreateInfo> queue_create_info;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
        for (uint32_t i = 0; i < queue_family_count; i++) {
            auto create_info =
                vk_init<VkDeviceQueueCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO);

            create_info.queueFamilyIndex = i;
            create_info.queueCount = 1;
            create_info.pQueuePriorities = &queue_priority;

            queue_create_info.push_back(create_info);
        }

        VkPhysicalDeviceFeatures device_features, enabled_features;
        m_physical_device.get_features(device_features);
        memset(&enabled_features, 0, sizeof(VkPhysicalDeviceFeatures));

        // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceFeatures.html
        // use the macro for every device feature used, preferably in specification order
#define ENABLE_DEVICE_FEATURE(feature) enabled_features.feature = device_features.feature
        ENABLE_DEVICE_FEATURE(geometryShader);
        ENABLE_DEVICE_FEATURE(samplerAnisotropy);
#undef ENABLE_DEVICE_FEATURE

        auto create_info = vk_init<VkDeviceCreateInfo>(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO);
        create_info.pQueueCreateInfos = queue_create_info.data();
        create_info.queueCreateInfoCount = queue_create_info.size();
        create_info.pEnabledFeatures = &enabled_features;

        if (!device_extensions.empty()) {
            create_info.ppEnabledExtensionNames = device_extensions.data();
            create_info.enabledExtensionCount = device_extensions.size();
        }

        if (!device_layers.empty()) {
            create_info.ppEnabledLayerNames = device_layers.data();
            create_info.enabledLayerCount = device_layers.size();
        }

        if (aftermath_info) {
            create_info.pNext = aftermath_info.get();
        }

        VkResult result = vkCreateDevice(physical_device, &create_info, nullptr, &m_device);
        check_vk_result(result);
    }
} // namespace sge
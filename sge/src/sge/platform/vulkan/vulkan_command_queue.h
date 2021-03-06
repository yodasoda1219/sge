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
#include "sge/renderer/command_queue.h"
namespace sge {
    class vulkan_command_queue : public command_queue {
    public:
        vulkan_command_queue(command_list_type type);
        virtual ~vulkan_command_queue() override;

        virtual command_list& get() override;
        virtual void submit(command_list& cmdlist, bool wait) override;

        virtual command_list_type get_type() override { return m_type; }

    private:
        struct stored_command_list {
            std::unique_ptr<command_list> cmdlist;
            VkFence fence;
        };

        command_list_type m_type;
        VkQueue m_queue;
        VkCommandPool m_command_pool;
        std::queue<stored_command_list> m_command_lists;
    };
} // namespace sge
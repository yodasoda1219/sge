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
#include "sge/core/directory_watcher.h"
#include "sge/core/window.h"
#include "sge/core/layer_stack.h"
#include "sge/events/event.h"
#include "sge/events/window_events.h"
#include "sge/renderer/swapchain.h"
namespace sge {
    enum subsystem : uint32_t {
        subsystem_input = 0x1,
        subsystem_asset = 0x2,
        subsystem_project = 0x4,
        subsystem_script_engine = 0x8
    };

    class imgui_layer;
    class application {
    public:
        struct entrypoint {
            int32_t operator()(int32_t argc, const char** argv);
        };

        static std::string get_engine_version();

        static void create();
        static void destroy();
        static application& get();

        application(const std::string& title);

        void push_layer(layer* _layer) { m_layer_stack.push_layer(_layer); }
        bool pop_layer(layer* _layer) { return m_layer_stack.pop_layer(_layer); }

        void push_overlay(layer* overlay) { m_layer_stack.push_overlay(overlay); }
        bool pop_overlay(layer* overlay) { return m_layer_stack.pop_overlay(overlay); }

        void quit() { m_running = false; }
        void on_event(event& e);

        void set_application_args(const std::vector<std::string>& args) { m_args = args; }
        void get_application_args(std::vector<std::string>& args) { args = m_args; }

        const std::string& get_title() { return m_title; }
        ref<window> get_window() { return m_window; }
        swapchain& get_swapchain() { return *m_swapchain; }
        imgui_layer& get_imgui_layer() { return *m_imgui_layer; }

        bool is_watching(const fs::path& path);
        bool watch_directory(const fs::path& path);
        bool remove_watched_directory(const fs::path& path);

        virtual bool is_editor() { return false; }
        bool is_subsystem_initialized(subsystem id) { return (m_initialized_subsystems & id) != 0; }

    protected:
        virtual void pre_init() {}
        virtual void on_init() {}
        virtual void on_shutdown() {}

        virtual std::string get_window_title() { return m_title; }
        virtual fs::path get_imgui_config_path() { return fs::path(); }
        virtual fs::path get_log_file_path() { return fs::path(); }

        void disable_subsystem(subsystem id);
        void reenable_subsystem(subsystem id);

        layer_stack m_layer_stack;
        std::string m_title;
        std::unordered_map<fs::path, std::unique_ptr<directory_watcher>, path_hasher> m_watchers;

        ref<window> m_window;
        std::unique_ptr<swapchain> m_swapchain;

        bool m_running, m_minimized;
        imgui_layer* m_imgui_layer = nullptr;

        std::vector<std::string> m_args;

    private:
        void init();
        void shutdown();
        void run();

        void init_logger();

        bool on_window_resize(window_resize_event& e);
        bool on_window_close(window_close_event& e);

        uint32_t m_disabled_subsystems = 0;
        uint32_t m_initialized_subsystems = 0;
    };
} // namespace sge
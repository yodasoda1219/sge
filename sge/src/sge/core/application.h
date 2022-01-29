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
#include "sge/core/window.h"
#include "sge/core/layer_stack.h"
#include "sge/events/event.h"
#include "sge/events/window_events.h"
#include "sge/renderer/swapchain.h"
namespace sge {
    class imgui_layer;
    class application {
    public:
        static void create();
        static void destroy();
        static application& get();

        application(const std::string& title);

        void push_layer(layer* _layer) { m_layer_stack.push_layer(_layer); }
        bool pop_layer(layer* _layer) { return m_layer_stack.pop_layer(_layer); }

        void push_overlay(layer* overlay) { m_layer_stack.push_overlay(overlay); }
        bool pop_overlay(layer* overlay) { return m_layer_stack.pop_overlay(overlay); }

        void init();
        void shutdown();
        void run();
        void quit() { m_running = false; }

        void on_event(event& e);

        const std::string& get_title() { return m_title; }
        ref<window> get_window() { return m_window; }
        swapchain& get_swapchain() { return *m_swapchain; }

    protected:
        virtual void on_init() {}
        virtual void on_shutdown() {}

        virtual std::string get_window_title() { return m_title; }
        virtual bool is_editor() { return false; }

        layer_stack m_layer_stack;
        std::string m_title;
        ref<window> m_window;
        std::unique_ptr<swapchain> m_swapchain;
        bool m_running, m_minimized;
        imgui_layer* m_imgui_layer = nullptr;

    private:
        bool on_window_resize(window_resize_event& e);
        bool on_window_close(window_close_event& e);
    };
} // namespace sge
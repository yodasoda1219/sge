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

#include "launcher_pch.h"
#include "launcher_layer.h"
#include <sge/core/main.h>

namespace sgm::launcher {
    class sgm_launcher : public application {
    public:
        sgm_launcher() : application("SGM Launcher") {
            static const std::vector<subsystem> disabled_subsystems = { subsystem_script_engine,
                                                                        subsystem_input,
                                                                        subsystem_project };

            for (subsystem id : disabled_subsystems) {
                disable_subsystem(id);
            }
        }

    protected:
        virtual void on_init() {
            launcher_layer::app_callbacks callbacks;
            callbacks.create_project = SGE_BIND_EVENT_FUNC(sgm_launcher::create_project);
            callbacks.open_project = SGE_BIND_EVENT_FUNC(sgm_launcher::open_project);

            m_layer = new launcher_layer(callbacks);
            push_layer(m_layer);
        }

        virtual void on_shutdown() {
            pop_layer(m_layer);
            delete m_layer;
        }

    private:
        void create_project(const launcher_layer::project_info& project_info) {
            // todo: implement
            spdlog::warn("not implemented yet");
        }

        void open_project(const fs::path& project_path) {
            spdlog::info("opening project: {0}", project_path.string());

            // todo: implement
            spdlog::warn("not implemented yet");
        }

        launcher_layer* m_layer;
    };
} // namespace sgm::launcher

application* create_app_instance() { return new sgm::launcher::sgm_launcher; }
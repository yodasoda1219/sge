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
#include "sge/asset/asset_manager.h"
namespace sge {
    class project {
    public:
        static void init();
        static void shutdown();

        static bool loaded();
        static project& get();

        static bool save();
        static bool load(const fs::path& path);

        project(const project&) = delete;
        project& operator=(const project&) = delete;

        asset_manager& get_asset_manager() { return *m_asset_manager; }

        const std::string& get_name() { return m_name; }
        const fs::path& get_path() { return m_path; }

        fs::path get_directory() { return m_path.parent_path(); }
        fs::path get_asset_dir() { return get_directory() / m_asset_dir; }
        fs::path get_start_scene() { return get_asset_dir() / m_start_scene; }

    private:
        project() = default;

        std::unique_ptr<asset_manager> m_asset_manager;
        fs::path m_path, m_asset_dir, m_start_scene;
        std::string m_name;
    };
} // namespace sge
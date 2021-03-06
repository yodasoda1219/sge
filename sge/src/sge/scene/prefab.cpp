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
#include "sge/scene/prefab.h"
#include "sge/scene/scene_serializer.h"
#include "sge/asset/project.h"

namespace sge {
    static entity_serializer s_prefab_serializer = entity_serializer(false);
    ref<prefab> prefab::from_entity(entity e, const fs::path& path) {
        json data;
        try {
            s_prefab_serializer.serialize(data, e);
        } catch (const std::exception&) {
            return nullptr;
        }

        fs::path asset_path = path;
        auto& _project = project::get();

        if (!asset_path.empty() && asset_path.is_relative()) {
            fs::path asset_dir = _project.get_asset_dir();
            asset_path = asset_dir / asset_path;
        }

        ref<prefab> result = new prefab(asset_path, data);
        if (!asset_path.empty()) {
            auto& registry = _project.get_asset_manager().registry;
            if (registry.contains(asset_path)) {
                registry.remove_asset(asset_path);
            }

            registry.register_asset(result);
            if (!serialize(result, asset_path)) {
                result.reset();
            }
        }

        return result;
    }

    bool prefab::serialize(ref<prefab> _prefab, const fs::path& path) {
        std::ofstream stream(path);
        if (!stream.is_open()) {
            return false;
        }

        stream << _prefab->m_data.dump(4) << std::flush;
        stream.close();

        return true;
    }

    bool prefab::reload() {
        if (!fs::exists(m_path)) {
            spdlog::error("attempted to load prefab from nonexistent file: {0}", m_path.string());
            return false;
        }

        std::ifstream stream(m_path);
        stream >> m_data;
        stream.close();

        return true;
    }

    entity prefab::instantiate(ref<scene> _scene) {
        return s_prefab_serializer.deserialize(m_data, _scene);
    }
}
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
#include "sge/asset/asset_serializers.h"
#include "sge/renderer/shader.h"
#include "sge/renderer/texture.h"
namespace sge {
    static std::unordered_map<asset_type, std::unique_ptr<asset_serializer>> asset_serializers;

    template <typename T>
    static void add_serializer(asset_type type) {
        static_assert(std::is_base_of_v<asset_serializer, T>,
                      "the given type must derive from asset_serializer!");

        if (asset_serializers.find(type) == asset_serializers.end()) {
            auto instance = new T;
            auto unique_ptr = std::unique_ptr<asset_serializer>(instance);

            asset_serializers.insert(std::make_pair(type, std::move(unique_ptr)));
        }
    }

    void asset_serializer::init() {
        add_serializer<shader_serializer>(asset_type::shader);
        add_serializer<texture2d_serializer>(asset_type::texture_2d);
    }

    bool asset_serializer::serialize(ref<asset> _asset) {
        if (!_asset ||
            (asset_serializers.find(_asset->get_asset_type()) == asset_serializers.end())) {
            return false;
        }

        try {
            auto& serializer = asset_serializers[_asset->get_asset_type()];
            return serializer->serialize_impl(_asset);
        } catch (const std::exception& exc) {
            spdlog::warn("error serializing asset at {0}: {1}", _asset->get_path().string(),
                         exc.what());
            return false;
        }
    }

    bool asset_serializer::deserialize(const asset_desc& desc, ref<asset>& _asset) {
        if (!desc.type.has_value() ||
            (asset_serializers.find(desc.type.value()) == asset_serializers.end())) {
            return false;
        }

        bool succeeded = false;
        try {
            auto& serializer = asset_serializers[desc.type.value()];
            succeeded = serializer->deserialize_impl(desc.path, _asset);
        } catch (const std::exception& exc) {
            spdlog::warn("error deserializing asset at {0}: {1}", desc.path.string(), exc.what());
        }

        if (succeeded && desc.id.has_value()) {
            _asset->id = desc.id.value();
        }

        return succeeded;
    }

    bool shader_serializer::deserialize_impl(const fs::path& path, ref<asset>& _asset) {
        if (!fs::exists(path)) {
            return false;
        }

        _asset = shader::create(path);
        return true;
    }

    bool texture2d_serializer::serialize_impl(ref<asset> _asset) {
        auto texture = _asset.as<texture_2d>();
        texture_2d::serialize_settings(texture);

        return true;
    }

    bool texture2d_serializer::deserialize_impl(const fs::path& path, ref<asset>& _asset) {
        if (!fs::exists(path)) {
            return false;
        }

        auto texture = texture_2d::load(path);
        if (texture) {
            _asset = texture;
            return true;
        }

        return false;
    }
} // namespace sge
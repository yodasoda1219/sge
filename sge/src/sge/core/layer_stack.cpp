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
#include "sge/core/layer_stack.h"
namespace sge {
    void layer_stack::push_layer(layer* _layer) {
        m_layers.emplace(m_layers.begin() + m_layer_insert_index, _layer);
        m_layer_insert_index++;
        _layer->on_attach();
    }

    void layer_stack::push_overlay(layer* overlay) {
        m_layers.emplace_back(overlay);
        overlay->on_attach();
    }

    layer* layer_stack::pop_layer(size_t index) {
        if (m_layer_insert_index == 0) {
            return nullptr;
        }

        size_t pop_index = index;
        if (pop_index == std::numeric_limits<size_t>::max()) {
            pop_index = m_layer_insert_index - 1;
        } else if (pop_index >= m_layer_insert_index) {
            return nullptr;
        }

        auto it = m_layers.begin() + pop_index;
        layer* _layer = it->get();
        _layer->on_detach();

        it->release();
        m_layers.erase(it);

        m_layer_insert_index--;
        return _layer;
    }

    bool layer_stack::pop_layer(layer* _layer) {
        if (m_layer_insert_index == 0) {
            return false;
        }

        std::optional<size_t> index;
        for (size_t i = 0; i < m_layer_insert_index; i++) {
            if (m_layers[i].get() == _layer) {
                index = i;
                break;
            }
        }
        if (!index.has_value()) {
            return false;
        }

        return pop_layer(index.value()) != nullptr;
    }

    layer* layer_stack::pop_overlay(size_t index) {
        size_t overlay_count = m_layers.size() - m_layer_insert_index;
        if (overlay_count == 0) {
            return nullptr;
        }

        size_t pop_index = index;
        if (pop_index == std::numeric_limits<size_t>::max()) {
            pop_index = overlay_count - 1;
        } else if (pop_index >= overlay_count) {
            return nullptr;
        }

        auto it = m_layers.begin() + m_layer_insert_index + pop_index;
        layer* overlay = it->get();
        overlay->on_detach();

        it->release();
        m_layers.erase(it);
        return overlay;
    }

    bool layer_stack::pop_overlay(layer* overlay) {
        size_t overlay_count = m_layers.size() - m_layer_insert_index;
        if (overlay_count == 0) {
            return false;
        }

        std::optional<size_t> index;
        for (size_t i = 0; i < overlay_count; i++) {
            if (m_layers[m_layer_insert_index + i].get() == overlay) {
                index = i;
            }
        }
        if (!index.has_value()) {
            return false;
        }

        return pop_overlay(index.value()) != nullptr;
    }

    void layer_stack::clear() {
        for (auto& _layer : m_layers) {
            _layer->on_detach();
        }
        m_layers.clear();
    }
} // namespace sge
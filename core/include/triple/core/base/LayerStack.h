#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include "Layer.h"

namespace triple::core {
	class LayerStack {
	public:
		void pushLayer(std::unique_ptr<Layer> layer) {
			m_layers.insert(m_layers.begin() + m_insertIndex, std::move(layer));
			m_insertIndex++;
		}

		void pushOverlay(std::unique_ptr<Layer> overlay) { m_layers.push_back(std::move(overlay)); }

		void popLayer(Layer *layer) {
			auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_insertIndex,
			                       [layer](auto &l) { return l.get() == layer; });
			if (it != m_layers.begin() + m_insertIndex) {
				(*it)->onDetach();
				m_layers.erase(it);
				m_insertIndex--;
			}
		}

		auto begin() { return m_layers.begin(); }
		auto end() { return m_layers.end(); }

		auto rbegin() { return m_layers.rbegin(); }
		auto rend() { return m_layers.rend(); }

	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		uint32_t m_insertIndex = 0;
	};
} // namespace triple::core
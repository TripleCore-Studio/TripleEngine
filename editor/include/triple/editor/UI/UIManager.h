#pragma once

#include <vector>
#include <memory>

#include "UIPanel.h"

namespace triple::editor {
	class UIManager {
	public:
		template <typename T, typename... Args>
		T *addPanel(Args &&...args) {
			auto panel = std::make_unique<T>(std::forward<Args>(args)...);
			T *ptr = panel.get();
			m_panels.push_back(std::move(panel));
			return ptr;
		}

		void render() {
			for (auto &panel : m_panels) {
				if (panel->visible) {
					panel->onRender();
				}
			}
		}

		void hideAll() {
			for (auto &panel : m_panels) {
				panel->visible = false;
			}
		}

		void showAll() {
			for (auto &panel : m_panels) {
				panel->visible = true;
			}
		}

	private:
		std::vector<std::unique_ptr<UIPanel>> m_panels;
	};
} // namespace triple::editor
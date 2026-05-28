#pragma once

#include <string>

namespace triple::editor {
	class UIPanel {
	public:
		explicit UIPanel(std::string title) : m_title(title) {}
		virtual void onRender() = 0;
		virtual ~UIPanel() = default;

		bool visible = true;

	protected:
		std::string m_title;
	};
} // namespace triple::editor
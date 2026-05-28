#pragma once

#include <triple/core/base/IWindow.h>

namespace triple::editor {
	class ImGuiLayer {
	public:
		~ImGuiLayer();
		void init(core::IWindow *window);
		void shutdown() const;
		void beginFrame();
		void endFrame();

	private:
		void stylesApply();
		void configureApply();
		bool m_initialized = false;
	};
} // namespace triple::editor
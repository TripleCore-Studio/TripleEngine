#pragma once

#include <triple/core/event/Event.h>

namespace triple::editor {
	class ImGuiLayer {
	public:
		~ImGuiLayer();
		void init();
		void shutdown() const;
		void beginFrame(float dt);
		void endFrame();
		void onEvent(core::Event &e) const;
		void resize(int width, int height);

		bool opened = true;

	private:
		void stylesApply();
		void configureApply();
		bool m_initialized = false;
	};
} // namespace triple::editor
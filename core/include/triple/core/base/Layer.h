#pragma once

#include "triple/core/event/Event.h"
#include "triple/core/base/EngineContext.h"

namespace triple::core {
	class Layer {
	public:
		virtual ~Layer() = default;

		// called only after Engine::run()
		virtual void onAttach(const EngineContext &ctx) = 0;
		virtual void onDetach() = 0;
		virtual void onUpdate(float dt) = 0;
		virtual void onRender(float t) = 0;
		virtual void onEvent(Event &e) = 0;
	};
} // namespace triple::core
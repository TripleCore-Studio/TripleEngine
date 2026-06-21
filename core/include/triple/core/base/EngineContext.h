#pragma once

#include <triple/gfx/IRenderer.h>

#include "triple/core/base/IWindow.h"
#include "triple/core/event/EventService.h"
#include "triple/core/input/InputSystem.h"

namespace triple::core {
	struct EngineContext {
		gfx::IRenderer *renderer = nullptr;
		IWindow *window = nullptr;
		EventService *eventService = nullptr;
		InputSystem *inputSystem = nullptr;
	};
} // namespace triple::core
#pragma once

#include <triple/gfx/IRenderer.h>

#include "triple/core/base/IWindow.h"
#include "triple/core/event/EventService.h"
#include "triple/core/input/InputSystem.h"
#include "triple/core/input/InputActionSystem.h"

namespace triple::core {
	struct EngineContext {
		gfx::IRenderer *renderer = nullptr;
		IWindow *window = nullptr;
		EventService *eventService = nullptr;
		InputSystem *inputSystem = nullptr;
		InputActionSystem *inputActionSystem = nullptr;
	};
} // namespace triple::core
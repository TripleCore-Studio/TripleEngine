#pragma once

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/FrameArena.h>

#include "triple/core/base/IWindow.h"
#include "triple/core/event/EventBus.h"
#include "triple/core/input/InputSystem.h"

namespace triple::core {
	struct EngineContext {
		gfx::IRenderer *renderer = nullptr;
		IWindow *window = nullptr;
		EventBus *bus = nullptr;
		InputSystem *inputSystem = nullptr;
		gfx::FrameArena *frameArena = nullptr;
	};
} // namespace triple::core
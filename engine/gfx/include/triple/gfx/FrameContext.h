#ifndef FRAME_CONTEXT_H
#define FRAME_CONTEXT_H

#include <vector>
#include "CameraData.h"
#include "RenderCommand.h"

namespace triple::gfx {
	struct FrameContext {
		std::vector<RenderCommand> commands;
		CameraData camera;
		float time = 0.0f;
	};
} // namespace triple::gfx

#endif // FRAME_CONTEXT_H

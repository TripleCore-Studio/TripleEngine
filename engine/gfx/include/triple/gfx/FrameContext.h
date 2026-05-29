#ifndef FRAME_CONTEXT_H
#define FRAME_CONTEXT_H

#include <vector>
#include "CameraData.h"
#include "RenderCommand.h"
#include "LightSources.h"

namespace triple::gfx {
	struct FrameContext {
		std::vector<RenderCommand> commands;
		CameraData camera;
		SunLight sunLight;
		CameraLight cameraLight;
		math::Vec3 ambientColor;
		float time = 0.0f;
	};
} // namespace triple::gfx

#endif // FRAME_CONTEXT_H

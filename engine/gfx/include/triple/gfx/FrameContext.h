#ifndef FRAME_CONTEXT_H
#define FRAME_CONTEXT_H

#include <vector>
#include "Mat4.h"
#include "CameraData.h"
#include "RenderCommand.h"

namespace TripleEngineCore::Graphics {
    struct FrameContext {
        std::vector<Graphics::RenderCommand> commands;
        CameraData camera;
        float time = 0.0f;
    };
}

#endif // FRAME_CONTEXT_H

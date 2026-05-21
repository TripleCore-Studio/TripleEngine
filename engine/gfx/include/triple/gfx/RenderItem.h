#ifndef RENDER_ITEM_H
#define RENDER_ITEM_H

#include "Core/CoreTypes.h"
#include "Graphics/RenderMaterial.h"

namespace TripleEngineCore::Graphics {
    struct RenderItem {
        GPUHandle geometry;
        uint32_t indexOffset;
        uint32_t indexCount;
        RenderMaterial material;
    };
}

#endif // RENDER_ITEM_H

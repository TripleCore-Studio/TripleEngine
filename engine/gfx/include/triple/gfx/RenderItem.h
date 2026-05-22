#ifndef RENDER_ITEM_H
#define RENDER_ITEM_H

#include "GfxTypes.h"
#include "RenderMaterial.h"

namespace triple::gfx {
    struct RenderItem {
        GPUHandle geometry;
        uint32_t indexOffset;
        uint32_t indexCount;
        RenderMaterial material;
    };
}

#endif // RENDER_ITEM_H

#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <vector>
#include "Graphics/Vertex.h"
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset {
    struct Primitive
    {
        uint32_t indexOffset;
        uint32_t indexCount;
        AssetID materialId = INVALID_ASSET_ID;
    };
}

#endif // PRIMITIVE_H

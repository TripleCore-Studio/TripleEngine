#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <triple/gfx/Vertex.h>
#include "Asset.h"

namespace triple::game {
	struct Primitive {
		uint32_t indexOffset;
		uint32_t indexCount;
		AssetID materialId = INVALID_ASSET_ID;
	};
} // namespace triple::game

#endif // PRIMITIVE_H

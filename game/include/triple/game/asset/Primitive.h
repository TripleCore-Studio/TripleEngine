#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "AssetTypes.h"
#include "Material.h"

namespace triple::game {
	struct Primitive {
		uint32_t indexOffset;
		uint32_t indexCount;
		TypedAssetID<Material> material;
	};
} // namespace triple::game

#endif // PRIMITIVE_H

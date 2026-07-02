#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "triple/game/asset/AssetTypes.h"
#include "triple/game/asset/Model.h"

namespace triple::game {
	struct MeshComponent {
		TypedAssetID<Model> model;
	};
} // namespace triple::game

#endif // MESH_COMPONENT_H
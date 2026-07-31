#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "triple/game/asset/AssetTypes.h"
#include "triple/game/asset/Model.h"
#include "triple/game/asset/MaterialInstance.h"

namespace triple::game {
	struct MeshRendererComponent {
		TypedAssetID<Model> model;
		std::vector<std::vector<MaterialInstance>> materialInstances;
	};
} // namespace triple::game

#endif // MESH_COMPONENT_H
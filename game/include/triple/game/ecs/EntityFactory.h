#pragma once

#include <entt/entt.hpp>

#include "triple/game/asset/Model.h"

#include "TransformComponent.h"

namespace triple::game {
	class AssetManager;

	class EntityFactory {
	public:
		EntityFactory(entt::registry &registry, AssetManager &assetManager)
		    : m_registry(registry), m_assetManager(assetManager) {}

		entt::entity createMeshEntity(TypedAssetID<Model> model, TransformComponent transform = {});

	private:
		entt::registry &m_registry;
		AssetManager &m_assetManager;
	};
} // namespace triple::game
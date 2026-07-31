#pragma once

#include <memory>

#include <entt/entt.hpp>

#include "triple/game/ecs/EntityFactory.h"

namespace triple::game {
	class AssetManager;

	class Scene {
	public:
		Scene(AssetManager *assets) : m_assets(assets) {
			m_factory = std::make_unique<EntityFactory>(m_registry, *assets);
		}
		entt::registry &registry() { return m_registry; }
		EntityFactory &entityFactory() { return *m_factory; }
		void onUpdate(float dt);

	private:
		entt::registry m_registry;
		std::unique_ptr<EntityFactory> m_factory;
		AssetManager *m_assets;
	};
} // namespace triple::game
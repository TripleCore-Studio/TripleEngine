#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class AssetManager;

	class Scene {
	public:
		Scene(AssetManager *assets) : m_assets(assets) {}
		entt::registry &getRegistry() { return m_registry; }
		void onUpdate(float dt);

	private:
		entt::registry m_registry;
		AssetManager *m_assets;
	};
} // namespace triple::game
#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class AssetService;

	class Scene {
	public:
		Scene(AssetService *assets) : m_assets(assets) {}
		entt::registry &getRegistry() { return m_registry; }
		void onUpdate(float dt);

	private:
		entt::registry m_registry;
		AssetService *m_assets;
	};
} // namespace triple::game
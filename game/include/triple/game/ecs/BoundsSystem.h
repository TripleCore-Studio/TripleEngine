#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class AssetManager;

	class BoundsSystem {
	public:
		static void onUpdate(entt::registry &registry, AssetManager *assets);
	};
} // namespace triple::game
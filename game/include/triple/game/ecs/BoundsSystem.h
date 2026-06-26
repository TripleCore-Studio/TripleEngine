#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class AssetService;

	class BoundsSystem {
	public:
		static void onUpdate(entt::registry &registry, AssetService *assets);
	};
} // namespace triple::game
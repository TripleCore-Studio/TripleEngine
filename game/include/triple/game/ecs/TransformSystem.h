#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class TransformSystem {
	public:
		static void onUpdate(entt::registry &reg, float dt);
	};
} // namespace triple::game
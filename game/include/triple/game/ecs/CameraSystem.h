#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class CameraSystem {
	public:
		static void onUpdate(entt::registry &reg, float dt);
	};
}; // namespace triple::game
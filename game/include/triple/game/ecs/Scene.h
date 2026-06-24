#pragma once

#include <entt/entt.hpp>

namespace triple::game {
	class Scene {
	public:
		entt::registry &getRegistry() { return m_registry; }
		void onUpdate(float dt);

	private:
		entt::registry m_registry;
	};
} // namespace triple::game
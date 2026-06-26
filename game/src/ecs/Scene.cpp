#include "triple/game/ecs/Scene.h"

#include "triple/game/ecs/TransformSystem.h"
#include "triple/game/ecs/CameraSystem.h"
#include "triple/game/ecs/BoundsSystem.h"

namespace triple::game {
	void Scene::onUpdate(float dt) {
		TransformSystem::onUpdate(m_registry, dt);
		CameraSystem::onUpdate(m_registry, dt);
		BoundsSystem::onUpdate(m_registry, m_assets);
	}
} // namespace triple::game
#include "triple/game/ecs/Scene.h"

#include "triple/game/ecs/TransformSystem.h"
#include "triple/game/ecs/CameraSystem.h"

namespace triple::game {
	void Scene::onUpdate(float dt) {
		TransformSystem::onUpdate(m_registry, dt);
		CameraSystem::onUpdate(m_registry, dt);
	}
} // namespace triple::game
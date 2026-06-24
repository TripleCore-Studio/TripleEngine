#include "triple/game/ecs/CameraSystem.h"

#include <triple/math/Mat4Operations.h>

#include "triple/game/ecs/TransformComponent.h"
#include "triple/game/ecs/CameraComponent.h"

namespace triple::game {
	void CameraSystem::onUpdate(entt::registry &reg, float dt) {
		auto view = reg.view<CameraComponent, TransformComponent>();
		for (auto [entity, cam, transform] : view.each()) {
			cam.projectionMatrix = triple::math::Mat4Operations::perspective(
			    cam.fov * 0.01745329251f, cam.aspectRatio, cam.nearPlane, cam.farPlane);
			cam.viewMatrix = triple::math::Mat4Operations::inverse(transform.worldMatrix);
		}
	}
} // namespace triple::game
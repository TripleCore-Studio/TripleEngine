#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include "Scene/CameraComponent.h"
#include "Scene/TransformComponent.h"
#include "Mat4.h"

namespace TripleEngineCore::Utils {
	TripleMath::Mat4 getProjectionMatrix(const Scene::CameraComponent& camera);
	TripleMath::Mat4 getViewMatrix(const Scene::TransformComponent& transform);
}

#endif // CAMERA_UTILS_H
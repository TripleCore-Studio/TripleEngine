#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

#include "Mat4.h"
#include "Scene/TransformComponent.h"

namespace TripleEngineCore::Utils {
	TripleMath::Mat4 getModelMatrix(const Scene::TransformComponent& t);
	TripleMath::Mat4 getRotationMatrix(const Scene::TransformComponent& t);
	TripleMath::Vec3 forward(const Scene::TransformComponent& t);
	TripleMath::Vec3 right(const Scene::TransformComponent& t);
	TripleMath::Vec3 up(const Scene::TransformComponent& t);
}

#endif // TRANSFORM_UTILS_H

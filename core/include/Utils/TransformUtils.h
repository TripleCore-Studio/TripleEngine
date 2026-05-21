#ifndef TRANSFORM_UTILS_H
#define TRANSFORM_UTILS_H

#include "ExportMacros.h"
#include "Mat4.h"
#include "Scene/TransformComponent.h"

namespace TripleEngineCore::Utils {
	CORE_API TripleMath::Mat4 getModelMatrix(const Scene::TransformComponent& t);
	CORE_API TripleMath::Mat4 getRotationMatrix(const Scene::TransformComponent& t);
	CORE_API TripleMath::Vec3 forward(const Scene::TransformComponent& t);
	CORE_API TripleMath::Vec3 right(const Scene::TransformComponent& t);
	CORE_API TripleMath::Vec3 up(const Scene::TransformComponent& t);
}

#endif // TRANSFORM_UTILS_H

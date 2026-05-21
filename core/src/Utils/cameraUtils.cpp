#include "Utils/CameraUtils.h"
#include "Mat4Operations.h"
#include "Utils/TransformUtils.h"

namespace TripleEngineCore::Utils {
	TripleMath::Mat4 getProjectionMatrix(const Scene::CameraComponent& camera)
	{
        return TripleMath::perspective(
            camera.fov * 0.01745329251f,
            camera.aspectRatio,
            camera.nearPlane,
            camera.farPlane
        );
	}
    TripleMath::Mat4 getViewMatrix(const Scene::TransformComponent& transform)
    {
        TripleMath::Vec3 eye = transform.position;
        TripleMath::Vec3 center = eye + Utils::forward(transform);
        TripleMath::Vec3 up = Utils::up(transform);
        return TripleEngineCore::TripleMath::lookAt(eye, center, up);
    }
}
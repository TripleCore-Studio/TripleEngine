#include "Scene/CameraComponent.h"
#include "Mat4Operations.h"
#include <cmath>

namespace TripleEngineCore::Scene {

    CameraComponent::CameraComponent()
        : fov(70.0f)
        , nearPlane(0.1f)
        , farPlane(100.0f)
        , aspectRatio(16.0f / 9.0f)
        , transform(nullptr)
    {
    }

    CameraComponent::CameraComponent(float fov, float nearPlane, float farPlane, float aspectRatio, TransformComponent* transform)
        : fov(fov)
        , nearPlane(nearPlane)
        , farPlane(farPlane)
        , aspectRatio(aspectRatio)
        , transform(transform)
    {
    }

    TripleMath::Mat4 CameraComponent::getProjectionMatrix() const {
        return TripleEngineCore::TripleMath::perspective(
            fov * 0.01745329251f,
            aspectRatio,
            nearPlane,
            farPlane
        );
    }

    TripleMath::Mat4 CameraComponent::getViewMatrix() const {
        if (!transform) return TripleMath::Mat4::identity();
        TripleMath::Vec3 eye = transform->position;
        TripleMath::Vec3 center = eye + transform->forward();
        TripleMath::Vec3 up = transform->up();
        return TripleEngineCore::TripleMath::lookAt(eye, center, up);
    }

} // namespace TripleEngineCore::Scene

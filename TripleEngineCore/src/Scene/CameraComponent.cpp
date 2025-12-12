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

    void CameraComponent::lookAt(const TripleMath::Vec3& target)
    {
        if (!transform) return;

        TripleMath::Vec3 dir = target - transform->position;
        if (dir.length() == 0.0f) return;
        dir = dir.normalized();
        float pitch = asin(dir.y) * 180.0f / 3.14159265f;
        float yaw = atan2(-dir.x, -dir.z) * 180.0f / 3.14159265f;
        transform->rotationEuler = TripleMath::Vec3(pitch, yaw, 0.0f);
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

    void CameraComponent::onUpdate(float dt) {
    }

} // namespace TripleEngineCore::Scene

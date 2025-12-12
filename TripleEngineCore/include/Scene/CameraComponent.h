#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Scene/Component.h"
#include "TransformComponent.h"
#include "Mat4.h"

namespace TripleEngineCore::Scene {

    class CameraComponent : public Component {
    public:
        CameraComponent();
        CameraComponent(float fov, float nearPlane, float farPlane, float aspectRatio, TransformComponent* transform);

        float fov;
        float nearPlane;
        float farPlane;
        float aspectRatio;

        TransformComponent* transform;

        void lookAt(const TripleMath::Vec3& target);

        TripleMath::Mat4 getProjectionMatrix() const;

        TripleMath::Mat4 getViewMatrix() const;

        void onUpdate(float dt) override;
    };

} // namespace TripleEngineCore::Scene

#endif // CAMERA_COMPONENT_H

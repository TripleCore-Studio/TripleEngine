#include "Scene/TransformComponent.h"
#include "Mat4Operations.h"

using namespace TripleEngineCore::TripleMath;

namespace TripleEngineCore::Scene {

    TripleMath::Mat4 TransformComponent::getModelMatrix() const {
        float rx = rotationEuler.x * 0.01745329251f;
        float ry = rotationEuler.y * 0.01745329251f;
        float rz = rotationEuler.z * 0.01745329251f;

        Mat4 rotX = rotate(rx, { 1, 0, 0 });
        Mat4 rotY = rotate(ry, { 0, 1, 0 });
        Mat4 rotZ = rotate(rz, { 0, 0, 1 });
        Mat4 t = translate(position);
		Mat4 s = TripleMath::scale(scale);

        // M = T * Rz * Ry * Rx * S
        return multiply(t, multiply(rotZ, multiply(rotY, multiply(rotX, s))));
    }

    TripleMath::Vec3 TransformComponent::forward() const {
        TripleMath::Mat4 rot = multiply(
            multiply(rotate(rotationEuler.z * 0.01745329251f, { 0, 0, 1 }),
                rotate(rotationEuler.y * 0.01745329251f, { 0, 1, 0 })),
            rotate(rotationEuler.x * 0.01745329251f, { 1, 0, 0 })
        );

        Vec4 fwd4(0, 0, -1, 0);
        Vec4 transformed = rot * fwd4;
        return Vec3(transformed.x, transformed.y, transformed.z);
    }

    TripleMath::Vec3 TransformComponent::right() const {
        TripleMath::Mat4 rot = multiply(
            multiply(rotate(rotationEuler.z * 0.01745329251f, { 0, 0, 1 }),
                rotate(rotationEuler.y * 0.01745329251f, { 0, 1, 0 })),
            rotate(rotationEuler.x * 0.01745329251f, { 1, 0, 0 })
        );

        Vec4 r4(1, 0, 0, 0);
        Vec4 transformed = rot * r4;
        return Vec3(transformed.x, transformed.y, transformed.z);
    }

    TripleMath::Vec3 TransformComponent::up() const {
        TripleMath::Mat4 rot = multiply(
            multiply(rotate(rotationEuler.z * 0.01745329251f, { 0, 0, 1 }),
                rotate(rotationEuler.y * 0.01745329251f, { 0, 1, 0 })),
            rotate(rotationEuler.x * 0.01745329251f, { 1, 0, 0 })
        );

        Vec4 u4(0, 1, 0, 0);
        Vec4 transformed = rot * u4;
        return Vec3(transformed.x, transformed.y, transformed.z);
    }
}
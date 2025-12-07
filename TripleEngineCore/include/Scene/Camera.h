#ifndef CAMERA_H
#define CAMERA_H

#include "Vec3.h"
#include "Mat4.h"

namespace TripleEngineCore::Scene {
    class Camera {
    public:
        TripleMath::Vec3 position{ 0, 0, 3 };
        TripleMath::Vec3 forward{ 0, 0, -1 };
        TripleMath::Vec3 up{ 0, 1, 0 };

        float fov = 70.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        float aspectRatio = 16.0f / 9.0f;

        TripleMath::Mat4 getViewMatrix() const;
        TripleMath::Mat4 getProjectionMatrix() const;
    };
}


#endif // CAMERA_H

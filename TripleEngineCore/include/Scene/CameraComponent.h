#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "ExportMacros.h"

namespace TripleEngineCore::Scene {
    struct CameraComponent {
        float fov;
        float nearPlane;
        float farPlane;
        float aspectRatio;
    };
} // namespace TripleEngineCore::Scene

#endif // CAMERA_COMPONENT_H

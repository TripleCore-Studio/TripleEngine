#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "ExportMacros.h"
#include <cstdint>

namespace TripleEngineCore::Scene {
    struct MeshComponent {
        uint32_t modelIndex = UINT32_MAX;
    };
}

#endif // MESH_COMPONENT_H
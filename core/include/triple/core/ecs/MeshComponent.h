#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include <cstdint>

namespace triple::core {
	struct MeshComponent {
		uint32_t modelIndex = UINT32_MAX;
	};
} // namespace triple::core

#endif // MESH_COMPONENT_H
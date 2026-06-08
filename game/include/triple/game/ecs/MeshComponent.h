#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include <cstdint>

namespace triple::game {
	struct MeshComponent {
		uint32_t modelIndex = UINT32_MAX;
	};
} // namespace triple::game

#endif // MESH_COMPONENT_H
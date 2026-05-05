#ifndef GEOMETRY_DESC_H
#define GEOMETRY_DESC_H

#include "Graphics/Vertex.h"
#include <cstdint>

namespace TripleEngineCore::Graphics {
	struct GeometryDesc {
		const Vertex* vertices;
		uint32_t vertexCount;
		const uint32_t* indices;
		uint32_t indexCount;
	};
}

#endif // GEOMETRY_DESC_H

#ifndef RUNTIME_MESH_H
#define RUNTIME_MESH_H

#include "Graphics/Vertex.h"
#include "RuntimeMaterial.h"
#include <cstdint>

namespace TripleEngineCore::Runtime
{
	struct RuntimeMesh
	{
	public:
		RuntimeMesh() : vertices(nullptr), indices(nullptr), material() {}
		RuntimeMesh(const Graphics::Vertex* vertices, size_t vertexCount, const uint32_t* indices,\
			size_t indexCount, RuntimeMaterial material, uint64_t geometryHash)
			: vertices(vertices),
			vertexCount(vertexCount),
			indices(indices),
			indexCount(indexCount),
			material(material),
			geometryHash(geometryHash) {}

		const Graphics::Vertex* vertices;
		size_t vertexCount = 0;
		const uint32_t* indices;
		size_t indexCount = 0;
		RuntimeMaterial material;

		uint64_t geometryHash = 0;
	};
}

#endif // RUNTIME_MESH_H

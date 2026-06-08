#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <triple/gfx/Vertex.h>
#include "Mesh.h"
#include "Asset.h"

namespace triple::game {
	struct Model : public Asset {
	public:
		Model() = default;

		std::vector<Mesh> meshes;
		std::vector<gfx::Vertex> vertices;
		std::vector<uint32_t> indices;

		Model(const Model &) = delete;
		Model &operator=(const Model &) = delete;

		Model(Model &&) = default;
		Model &operator=(Model &&) = default;
	};
} // namespace triple::game

#endif // MODEL_H

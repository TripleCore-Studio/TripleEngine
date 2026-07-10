#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <limits>

#include <triple/math/Vec3.h>

#include "Mesh.h"
#include "AssetTypes.h"
#include "LoadersParams.h"

namespace triple::game {
	struct Model {
	public:
		Model() = default;

		std::vector<uint8_t> vertices;
		gfx::VertexLayout vertexLayout;

		std::vector<Mesh> meshes;
		std::vector<uint32_t> indices;

		math::Vec3 boundsMin{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
		                     std::numeric_limits<float>::max()};
		math::Vec3 boundsMax{-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
		                     -std::numeric_limits<float>::max()};
	};

	template <>
	struct AssetTypeOf<Model> {
		static constexpr AssetType kValue = AssetType::Model;
	};

	template <>
	struct LoadParamsOf<Model> {
		using Type = ModelLoadParams;
	};

} // namespace triple::game

#endif // MODEL_H

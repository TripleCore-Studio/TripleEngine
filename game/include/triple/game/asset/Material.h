#ifndef MATERIAL_H
#define MATERIAL_H

#include <triple/math/Vec4.h>

#include "AssetTypes.h"

#include "Texture.h"
#include "Shader.h"

namespace triple::game {
	struct Material {
	public:
		Material() = default;

		triple::math::Vec4 albedoColor = {1, 1, 1, 1};
		float metallic = 0.0f;
		float roughness = 1.0f;

		TypedAssetID<Texture> albedoTexture;
		TypedAssetID<Texture> normalTexture;
		TypedAssetID<Texture> metallicTexture;
		TypedAssetID<Texture> roughnessTexture;

		TypedAssetID<Shader> shader;

		Material(const Material &other) = default;
		Material &operator=(const Material &other) = default;
	};

	template <>
	struct AssetTypeOf<Material> {
		static constexpr AssetType kValue = AssetType::Material;
	};
} // namespace triple::game

#endif // MATERIAL_H

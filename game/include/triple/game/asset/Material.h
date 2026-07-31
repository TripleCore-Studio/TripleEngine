#ifndef MATERIAL_H
#define MATERIAL_H

#include <array>
#include <unordered_map>

#include <triple/math/Vec4.h>

#include "AssetTypes.h"
#include "LoadersParams.h"
#include "Texture.h"
#include "Shader.h"

namespace triple::game {
	struct MaterialParamValue {
		std::array<float, 16> data{};
	};

	enum class MaterialBlendMode : uint8_t {
		Opaque,
		AlphaCutoff,
		Transparent,
	};

	struct Material {
	public:
		TypedAssetID<Shader> shader;
		MaterialBlendMode blendMode;
		std::unordered_map<std::string, MaterialParamValue> params;
		std::unordered_map<std::string, TypedAssetID<Texture>> textures;
	};

	template <>
	struct AssetTypeOf<Material> {
		static constexpr AssetType kValue = AssetType::Material;
	};

	template <>
	struct LoadParamsOf<Material> {
		using Type = MaterialLoadParams;
	};
} // namespace triple::game

#endif // MATERIAL_H

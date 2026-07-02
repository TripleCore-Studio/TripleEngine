#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "AssetTypes.h"
#include "LoadersParams.h"

namespace triple::game {

	struct Shader {
		std::string vertexSource;
		std::string fragmentSource;
	};

	template <>
	struct AssetTypeOf<Shader> {
		static constexpr AssetType kValue = AssetType::Shader;
	};

	template <>
	struct LoadParamsOf<Shader> {
		using Type = ShaderLoadParams;
	};
} // namespace triple::game

#endif // SHADER_H

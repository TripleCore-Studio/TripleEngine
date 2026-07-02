#pragma once

#include <string>

namespace triple::game {
	template <typename T>
	struct LoadParamsOf;

	struct ModelLoadParams {
		std::string path;
	};
	struct ShaderLoadParams {
		std::string vertexPath;
		std::string fragmentPath;
	};
	struct TextureLoadParams {
		std::string path;
	};
} // namespace triple::game
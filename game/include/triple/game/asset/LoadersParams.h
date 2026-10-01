#pragma once

#include <string>

namespace triple::game {
	template <typename T>
	struct LoadParamsOf;

	struct ModelLoadParams {
		std::string path;
	};
	struct ShaderLoadParams {
		std::string basePath;
		// to request a JSON file
		bool requireDescriptor = true;
	};
	struct TextureLoadParams {
		std::string path;
	};
	struct MaterialLoadParams {
		std::string path;
	};
} // namespace triple::game

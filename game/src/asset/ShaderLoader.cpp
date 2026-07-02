#include "triple/game/asset/ShaderLoader.h"

#include <fstream>
#include <sstream>

#include <triple/log/Logger.h>

namespace triple::game {
	std::optional<Shader> ShaderLoader::load(const ShaderLoadParams &params) {
		std::ifstream vsFile(params.vertexPath);
		std::ifstream fsFile(params.fragmentPath);
		if (!vsFile.is_open()) {
			triple::log::Logger::ModuleError("ShaderLoader", "Failed to open vertex shader '{}'",
			                                 params.vertexPath);
			return std::nullopt;
		}
		if (!fsFile.is_open()) {
			triple::log::Logger::ModuleError("ShaderLoader", "Failed to open fragment shader '{}'",
			                                 params.fragmentPath);
			return std::nullopt;
		}

		std::stringstream vsStream, fsStream;
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();

		Shader shader;
		shader.vertexSource = vsStream.str();
		shader.fragmentSource = fsStream.str();

		return std::move(shader);
	}
} // namespace triple::game
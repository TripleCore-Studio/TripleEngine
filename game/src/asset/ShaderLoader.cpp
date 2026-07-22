#include "triple/game/asset/ShaderLoader.h"

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include <triple/log/Logger.h>

#include <triple/gfx/DrawCommand.h>
#include <triple/gfx/UniformAlignment.h>

namespace triple::game {
	gfx::UniformType parseUniformType(const std::string &s) {
		if (s == "Float")
			return gfx::UniformType::Float;
		if (s == "Vec2")
			return gfx::UniformType::Vec2;
		if (s == "Vec3")
			return gfx::UniformType::Vec3;
		if (s == "Vec4")
			return gfx::UniformType::Vec4;
		if (s == "Mat4")
			return gfx::UniformType::Mat4;
		if (s == "Int")
			return gfx::UniformType::Int;

		triple::log::Logger::moduleError("ShaderLoader", "Unknown uniform type '{}'", s);
		return gfx::UniformType::Float;
	}

	gfx::VertexAttribType parseVertexAttribType(const std::string &s) {
		if (s == "Float")
			return gfx::VertexAttribType::Float;
		if (s == "Vec2")
			return gfx::VertexAttribType::Vec2;
		if (s == "Vec3")
			return gfx::VertexAttribType::Vec3;
		if (s == "Vec4")
			return gfx::VertexAttribType::Vec4;

		triple::log::Logger::moduleError("ShaderLoader", "Unknown vertex attribute type '{}'", s);
		return gfx::VertexAttribType::Float;
	}

	uint32_t vertexAttribTypeSize(gfx::VertexAttribType type) {
		switch (type) {
			case gfx::VertexAttribType::Float:
				return sizeof(float);
			case gfx::VertexAttribType::Vec2:
				return sizeof(float) * 2;
			case gfx::VertexAttribType::Vec3:
				return sizeof(float) * 3;
			case gfx::VertexAttribType::Vec4:
				return sizeof(float) * 4;
		}
		return sizeof(float);
	}

	bool parseAttributes(const nlohmann::json &json, Shader &shader) {
		if (!json.contains("attributes")) {
			return true;
		}

		gfx::VertexLayout &layout = shader.desc.vertexLayout;

		uint32_t location = 0;
		uint32_t offset = 0;
		for (const auto &attr : json.at("attributes")) {
			if (!attr.contains("semantic") || !attr.contains("type")) {
				triple::log::Logger::moduleError("ShaderLoader",
				                                 "Attribute entry missing 'semantic' or 'type'");
				return false;
			}

			gfx::VertexAttributeDesc desc;
			desc.semantic = attr.at("semantic").get<std::string>();
			desc.type = parseVertexAttribType(attr.at("type").get<std::string>());
			desc.location = location++;
			desc.offset = offset;

			offset += vertexAttribTypeSize(desc.type);

			layout.attributes.push_back(std::move(desc));
		}

		layout.stride = offset;

		return true;
	}

	bool parseUniforms(const nlohmann::json &json, Shader &shader) {
		if (!json.contains("uniforms")) {
			return true;
		}

		uint32_t offset = 0;
		for (const auto &u : json.at("uniforms")) {
			if (!u.contains("name") || !u.contains("type")) {
				triple::log::Logger::moduleError("ShaderLoader",
				                                 "Uniform entry missing 'name' or 'type'");
				return false;
			}

			gfx::UniformParamDesc desc;
			desc.name = u.at("name").get<std::string>();
			desc.type = parseUniformType(u.at("type").get<std::string>());

			// TODO: std140 packing is GL-specific; Vulkan/HLSL backends need their own alignment
			// rule here.
			offset = gfx::alignStd140(desc.type, offset);
			desc.offset = offset;
			offset += gfx::sizeOfUniform(desc.type);

			shader.desc.uniforms.push_back(std::move(desc));
		}

		shader.desc.uniformBlockSize =
		    gfx::alignStd140(gfx::UniformType::Vec4, offset); // whole block rounds up to 16
		return true;
	}

	bool parseTextureSlots(const nlohmann::json &json, Shader &shader) {
		if (!json.contains("textureSlots")) {
			return true;
		}

		for (const auto &t : json.at("textureSlots")) {
			if (!t.contains("name") || !t.contains("slot")) {
				triple::log::Logger::moduleError("ShaderLoader",
				                                 "Texture slot entry missing 'name' or 'slot'");
				return false;
			}

			gfx::TextureSlotDesc desc;
			desc.name = t.at("name").get<std::string>();
			desc.slot = t.at("slot").get<uint32_t>();

			if (desc.slot >= gfx::kMaxTextureSlots) {
				triple::log::Logger::moduleError(
				    "ShaderLoader", "Texture slot '{}' exceeds kMaxTextureSlots", desc.name);
				return false;
			}

			shader.desc.textureSlots.push_back(std::move(desc));
		}

		return true;
	}

	std::optional<Shader> ShaderLoader::load(const ShaderLoadParams &params) {
		const std::string kVertexPath = params.basePath + ".vert";
		const std::string kFragmentPath = params.basePath + ".frag";
		const std::string kJsonPath = params.basePath + ".shader.json";

		std::ifstream vsFile(kVertexPath);
		std::ifstream fsFile(kFragmentPath);
		std::ifstream jsonFile(kJsonPath);

		if (!vsFile.is_open()) {
			triple::log::Logger::moduleError("ShaderLoader", "Failed to open vertex shader '{}'",
			                                 kVertexPath);
			return std::nullopt;
		}
		if (!fsFile.is_open()) {
			triple::log::Logger::moduleError("ShaderLoader", "Failed to open fragment shader '{}'",
			                                 kFragmentPath);
			return std::nullopt;
		}
		if (!jsonFile.is_open()) {
			triple::log::Logger::moduleError("ShaderLoader",
			                                 "Failed to open shader descriptor '{}'", kJsonPath);
			return std::nullopt;
		}

		std::stringstream vsStream, fsStream;
		vsStream << vsFile.rdbuf();
		fsStream << fsFile.rdbuf();

		nlohmann::json json;
		try {
			jsonFile >> json;
		} catch (const nlohmann::json::parse_error &e) {
			triple::log::Logger::moduleError("ShaderLoader", "Failed to parse '{}': {}", kJsonPath,
			                                 e.what());
			return std::nullopt;
		}

		Shader shader;
		shader.desc.vertexSource = vsStream.str();
		shader.desc.fragmentSource = fsStream.str();

		if (!parseAttributes(json, shader) || !parseUniforms(json, shader) ||
		    !parseTextureSlots(json, shader)) {
			triple::log::Logger::moduleError("ShaderLoader",
			                                 "Failed to parse shader descriptor '{}'", kJsonPath);
			return std::nullopt;
		}

		return shader;
	}
} // namespace triple::game
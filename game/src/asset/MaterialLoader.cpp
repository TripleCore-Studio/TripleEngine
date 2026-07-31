#include "triple/game/asset/MaterialLoader.h"

#include <algorithm>
#include <fstream>
#include <optional>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <triple/log/Logger.h>

#include "triple/game/asset/Shader.h"
#include "triple/game/asset/Texture.h"
#include "triple/game/asset/AssetManager.h"
#include "triple/game/asset/DefaultAssets.h"

namespace triple::game {
	static bool parseShader(const nlohmann::json &json, Material &material,
	                        AssetManager &assetManager, const std::string &jsonPath);
	static bool parseBlendMode(const nlohmann::json &json, Material &material,
	                           const std::string &jsonPath);
	static bool parseParams(const nlohmann::json &json, Material &material,
	                        const std::string &jsonPath, AssetManager &manager);
	static bool parseTextures(const nlohmann::json &json, Material &material,
	                          const std::string &jsonPath, AssetManager &manager);

	std::optional<Material> MaterialLoader::load(const MaterialLoadParams &params) {
		const std::string kJsonPath = params.path;

		std::ifstream jsonFile(kJsonPath);
		if (jsonFile.is_open()) {
			triple::log::Logger::moduleError("MaterialLoader",
			                                 "Failed to open material descriptor '{}'", kJsonPath);
			return std::nullopt;
		}

		nlohmann::json json;
		try {
			jsonFile >> json;
		} catch (const nlohmann::json::parse_error &e) {
			triple::log::Logger::moduleError("MaterialLoader", "Failed to parse '{}': {}",
			                                 kJsonPath, e.what());
			return std::nullopt;
		}

		Material material;

		if (!parseShader(json, material, *m_assetManager, kJsonPath))
			return std::nullopt;

		if (!parseBlendMode(json, material, kJsonPath))
			return std::nullopt;

		if (!parseParams(json, material, kJsonPath, *m_assetManager))
			return std::nullopt;

		if (!parseTextures(json, material, kJsonPath, *m_assetManager))
			return std::nullopt;

		return material;
	}

	static std::optional<TypedAssetID<Shader>> getOrLoadShader(const std::string &shaderPath,
	                                                           AssetManager &manager) {
		auto &storage = manager.storageFor<Shader>();

		TypedAssetID<Shader> existing = storage.findByName(shaderPath);
		if (storage.contains(existing.raw)) {
			return existing;
		}

		return manager.load<Shader>(shaderPath, ShaderLoadParams{shaderPath});
	}

	static bool parseShader(const nlohmann::json &json, Material &material,
	                        AssetManager &assetManager, const std::string &jsonPath) {
		if (!json.contains("shader") || !json["shader"].is_string()) {
			triple::log::Logger::moduleError("MaterialLoader",
			                                 "'{}': missing or invalid 'shader' field", jsonPath);
			return false;
		}

		std::string shaderPath = json["shader"].get<std::string>();
		auto shaderId = getOrLoadShader(shaderPath, assetManager);

		if (!shaderId) {
			triple::log::Logger::moduleError("MaterialLoader",
			                                 "'{}': failed to load referenced shader '{}'",
			                                 jsonPath, shaderPath);
			return false;
		}

		material.shader = *shaderId;
		return true;
	}

	static bool parseBlendMode(const nlohmann::json &json, Material &material,
	                           const std::string &jsonPath) {
		static const std::unordered_map<std::string, MaterialBlendMode> kBlendModes = {
		    {"Opaque", MaterialBlendMode::Opaque},
		    {"AlphaCutoff", MaterialBlendMode::AlphaCutoff},
		    {"Transparent", MaterialBlendMode::Transparent},
		};

		std::string modeStr = json.value("blendMode", "Opaque");
		auto it = kBlendModes.find(modeStr);
		if (it == kBlendModes.end()) {
			triple::log::Logger::moduleError("MaterialLoader", "'{}': unknown blendMode '{}'",
			                                 jsonPath, modeStr);
			return false;
		}
		material.blendMode = it->second;
		return true;
	}

	static bool parseParams(const nlohmann::json &json, Material &material,
	                        const std::string &jsonPath, AssetManager &manager) {
		if (!json.contains("params"))
			return true;

		const Shader *shader = manager.storageFor<Shader>().get(material.shader);

		for (auto &[name, value] : json["params"].items()) {
			MaterialParamValue paramValue{};

			if (value.is_number()) {
				paramValue.data[0] = value.get<float>();
			} else if (value.is_array()) {
				size_t count = std::min(value.size(), paramValue.data.size());
				for (size_t i = 0; i < count; ++i) {
					paramValue.data[i] = value[i].get<float>();
				}
			} else {
				triple::log::Logger::moduleError(
				    "MaterialLoader", "'{}': param '{}' has unsupported JSON type", jsonPath, name);
				continue;
			}

			if (shader) {
				bool knownUniform =
				    std::any_of(shader->desc.uniforms.begin(), shader->desc.uniforms.end(),
				                [&](const auto &u) { return u.name == name; });
				if (!knownUniform) {
					triple::log::Logger::moduleError(
					    "MaterialLoader",
					    "'{}': param '{}' not declared in shader uniforms — ignored at pack time",
					    jsonPath, name);
				}
			}

			material.params[name] = paramValue;
		}
		return true;
	}

	[[nodiscard]] TypedAssetID<Texture> defaultTextureForSlot(const std::string &slotName,
	                                                          AssetManager &manager) {
		static const std::unordered_map<std::string, std::string_view> kSlotToDefaultTexture = {
		    {std::string(kAlbedoMapSlot), kDefaultAlbedoTextureName},
		    {std::string(kMetallicMapSlot), kDefaultMetallicTextureName},
		    {std::string(kNormalMapSlot), kDefaultNormalTextureName},
		    {std::string(kRoughnessMapSlot), kDefaultRoughnessTextureName},
		};

		auto it = kSlotToDefaultTexture.find(slotName);
		std::string_view defaultName =
		    (it != kSlotToDefaultTexture.end()) ? it->second : kDefaultAlbedoTextureName;

		return manager.storageFor<Texture>().findByName(std::string(defaultName));
	}

	static bool parseTextures(const nlohmann::json &json, Material &material,
	                          const std::string &jsonPath, AssetManager &manager) {
		const Shader *shader = manager.storageFor<Shader>().get(material.shader);
		if (!shader)
			return true;

		const nlohmann::json *texturesJson =
		    json.contains("textures") ? &json["textures"] : nullptr;

		for (const auto &slot : shader->desc.textureSlots) {
			std::optional<std::string> texPath;

			if (texturesJson && texturesJson->contains(slot.name) &&
			    (*texturesJson)[slot.name].is_string()) {
				texPath = (*texturesJson)[slot.name].get<std::string>();
			}

			TypedAssetID<Texture> texId;

			if (texPath) {
				auto &storage = manager.storageFor<Texture>();
				TypedAssetID<Texture> existing = storage.findByName(*texPath);

				if (storage.contains(existing.raw)) {
					texId = existing;
				} else {
					auto loaded = manager.load<Texture>(*texPath, TextureLoadParams{*texPath});
					if (loaded) {
						texId = *loaded;
					} else {
						triple::log::Logger::moduleError(
						    "MaterialLoader",
						    "'{}': failed to load texture '{}' for slot '{}', using default",
						    jsonPath, *texPath, slot.name);
						texId = defaultTextureForSlot(slot.name, manager);
					}
				}
			} else {
				texId = defaultTextureForSlot(slot.name, manager);
			}

			material.textures[slot.name] = texId;
		}

		return true;
	}
} // namespace triple::game
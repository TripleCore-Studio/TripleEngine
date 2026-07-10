#pragma once

#include <string_view>

namespace triple::game {
	constexpr std::string_view kDefaultAlbedoTextureName = "default_albedo";
	constexpr std::string_view kDefaultNormalTextureName = "default_normal";
	constexpr std::string_view kDefaultMetallicTextureName = "default_metallic";
	constexpr std::string_view kDefaultRoughnessTextureName = "default_roughness";

	constexpr std::string_view kDefaultShaderName = "default_shader";

	constexpr std::string_view kDefaultMaterialName = "default_material";

	constexpr std::string_view kAlbedoColorParam = "albedoColor";
	constexpr std::string_view kMetallicParam = "metallic";
	constexpr std::string_view kRoughnessParam = "roughness";
	constexpr std::string_view kAlbedoMapSlot = "albedoMap";
	constexpr std::string_view kMetallicMapSlot = "metallicMap";
	constexpr std::string_view kNormalMapSlot = "normalMap";
	constexpr std::string_view kRoughnessMapSlot = "roughnessMap";
} // namespace triple::game
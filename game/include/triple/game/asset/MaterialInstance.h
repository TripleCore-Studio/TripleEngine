#pragma once

#include <string>
#include <unordered_map>

#include "Material.h"
#include "Texture.h"
#include "AssetTypes.h" // TypedAssetID

namespace triple::game {

	class MaterialInstance {
	public:
		explicit MaterialInstance(TypedAssetID<Material> base) : m_base(base) {}

		[[nodiscard]] TypedAssetID<Material> base() const { return m_base; }

		void setFloat(const std::string &name, float value) {
			m_paramOverrides[name] = MaterialParamValue{{value}};
		}

		void setVec2(const std::string &name, float x, float y) {
			m_paramOverrides[name] = MaterialParamValue{{x, y}};
		}

		void setVec3(const std::string &name, float x, float y, float z) {
			m_paramOverrides[name] = MaterialParamValue{{x, y, z}};
		}

		void setVec4(const std::string &name, float x, float y, float z, float w) {
			m_paramOverrides[name] = MaterialParamValue{{x, y, z, w}};
		}

		void setTexture(const std::string &slotName, TypedAssetID<Texture> texture) {
			m_textureOverrides[slotName] = texture;
		}

		void clearParamOverride(const std::string &name) { m_paramOverrides.erase(name); }
		void clearTextureOverride(const std::string &slotName) {
			m_textureOverrides.erase(slotName);
		}

		[[nodiscard]] const std::unordered_map<std::string, MaterialParamValue> &
		paramOverrides() const {
			return m_paramOverrides;
		}
		[[nodiscard]] const std::unordered_map<std::string, TypedAssetID<Texture>> &
		textureOverrides() const {
			return m_textureOverrides;
		}

	private:
		TypedAssetID<Material> m_base;
		std::unordered_map<std::string, MaterialParamValue> m_paramOverrides;
		std::unordered_map<std::string, TypedAssetID<Texture>> m_textureOverrides;
	};

} // namespace triple::game
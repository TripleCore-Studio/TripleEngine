#pragma once

#include "ILoader.h"
#include "Material.h"

namespace triple::game {
	class AssetManager;

	class MaterialLoader : public ILoader<Material> {
	public:
		MaterialLoader(AssetManager *manager) : m_assetManager(manager) {}
		std::optional<Material> load(const MaterialLoadParams &params) override;

	private:
		AssetManager *m_assetManager;
	};
} // namespace triple::game
#pragma once

#include "ILoader.h"
#include "Model.h"

namespace triple::game {
	class AssetManager;

	class ModelLoader : public ILoader<Model> {
	public:
		ModelLoader(AssetManager *manager) : m_manager(manager) {}
		std::optional<Model> load(const ModelLoadParams &params) override;

	private:
		AssetManager *m_manager;
	};
} // namespace triple::game
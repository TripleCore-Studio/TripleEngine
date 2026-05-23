#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "triple/core/base/ISystem.h"

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/RenderCommand.h>

#include "triple/core/ecs/Scene.h"
#include "triple/core/asset/AssetService.h"
#include "triple/core/asset/Asset.h"

#include <vector>
#include <unordered_map>

namespace triple::core {
	class RenderSystem : public ISystem {
	public:
		RenderSystem(AssetService* assetsSrv) : m_assets(assetsSrv) {}
		RenderSystem() = default;
		~RenderSystem() = default;

		void update(float dt) override {};
		void init() override {}
		void shutdown() override {}
		[[nodiscard]] SystemPriority priority() const override { return SystemPriority::Late; }

		void uploadTexture(const Texture* texture);
		void uploadGeometry(const Model* model);
		void uploadShader(const Shader* shader);

		void buildRenderCommands(Scene* scene, std::vector<gfx::RenderCommand>& cmd);
		void setRenderer(gfx::IRenderer* renderer) { this->m_renderer = renderer; }
		gfx::IRenderer* getRenderer() { return this->m_renderer; }
	private:
		enum class ResourceType { Texture, Model, Shader };
		bool getGPU(ResourceType type, AssetID id, gfx::GPUHandle& out);

		void gatherFromEntity(
			Scene* scene,
			Entity e,
			std::vector<gfx::RenderCommand>& commands,
			const triple::math::Mat4& parentWorld);

		void buildRenderCmd(gfx::RenderCommand& cmd, const Model* obj);

		AssetService* m_assets = nullptr;
		gfx::IRenderer* m_renderer = nullptr;

		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedTextures;
		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedModels;
		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedShaders;
	};
}

#endif // RENDER_SYSTEM_H

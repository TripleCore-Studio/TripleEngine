#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>
#include <unordered_map>

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/RenderCommand.h>

#include <triple/game/asset/AssetService.h>
#include <triple/game/asset/Asset.h>

#include "triple/game/ecs/Scene.h"

namespace triple::game {
	class RenderSystem {
	public:
		RenderSystem(AssetService *assetsSrv) : m_assets(assetsSrv) {}
		RenderSystem() = default;
		~RenderSystem() = default;

		void update(float dt) {};
		void init() {}
		void shutdown() {}

		void uploadTexture(const Texture *texture);
		void uploadGeometry(const Model *model);
		void uploadShader(const Shader *shader);

		void buildRenderCommands(Scene *scene, std::vector<gfx::RenderCommand> &cmd);
		void setRenderer(gfx::IRenderer *renderer) { this->m_renderer = renderer; }
		gfx::IRenderer *getRenderer() { return this->m_renderer; }

	private:
		enum class ResourceType { Texture, Model, Shader };
		bool getGPU(ResourceType type, AssetID id, gfx::GPUHandle &out);

		void gatherFromEntity(Scene *scene, Entity e, std::vector<gfx::RenderCommand> &commands,
		                      const triple::math::Mat4 &parentWorld);

		void buildRenderCmd(gfx::RenderCommand &cmd, const Model *obj);

		AssetService *m_assets = nullptr;
		gfx::IRenderer *m_renderer = nullptr;

		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedTextures;
		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedModels;
		std::unordered_map<AssetID, gfx::GPUHandle> m_uploadedShaders;
	};
} // namespace triple::game

#endif // RENDER_SYSTEM_H

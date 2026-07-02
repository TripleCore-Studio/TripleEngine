#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <vector>

#include <entt/entt.hpp>

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/RenderCommand.h>

namespace triple::game {
	class Model;
	class GpuResourceRegistry;
	class AssetManager;

	class RenderSystem {
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		static void buildRenderCommands(entt::registry &reg, std::vector<gfx::RenderCommand> &cmd);

		static void setRenderer(gfx::IRenderer *r) { s_renderer = r; }
		static void setRegistry(GpuResourceRegistry *r) { s_registry = r; }
		static void setAssetManager(AssetManager *m) { s_assetManager = m; }

	private:
		static void buildRenderCmd(gfx::RenderCommand &cmd, const Model *model,
		                           gfx::GPUHandle geometryHandle);

	private:
		inline static gfx::IRenderer *s_renderer = nullptr;
		inline static GpuResourceRegistry *s_registry = nullptr;
		inline static AssetManager *s_assetManager = nullptr;
	};
} // namespace triple::game

#endif // RENDER_SYSTEM_H

#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <entt/entt.hpp>

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/DrawCommand.h>
#include <triple/gfx/FrameArena.h>

#include "triple/game/asset/Primitive.h"
#include "triple/game/asset/MaterialInstance.h"

namespace triple::game {
	class Model;
	class GpuResourceRegistry;
	class AssetManager;

	class RenderSystem {
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		static void setRenderer(gfx::IRenderer *r) { s_renderer = r; }
		static void setRegistry(GpuResourceRegistry *r) { s_registry = r; }
		static void setAssetManager(AssetManager *m) { s_assetManager = m; }
		static void setFullscreenQuad(gfx::GeometryHandle quad, uint32_t indexCount) {
			s_fullscreenQuad = quad;
			s_fullscreenQuadIndexCount = indexCount;
		}

		static void submitScene(
		    entt::registry &registry,
		    gfx::ViewHandle opaqueView,
		    gfx::ViewHandle transparentView,
		    gfx::FrameArena &arena,
		    const math::Vec3 &cameraPosition
		);

		static void submitFullscreenPass(
		    gfx::ViewHandle view,
		    gfx::RenderPass pass,
		    std::string_view shaderName,
		    std::initializer_list<gfx::TextureHandle> textures
		);

	private:
		static void submitPrimitive(
		    const Primitive &prim,
		    const MaterialInstance &instance,
		    gfx::GeometryHandle geometry,
		    const math::Mat4 &worldMatrix,
		    gfx::ViewHandle opaqueView,
		    gfx::ViewHandle transparentView,
		    gfx::FrameArena &arena,
		    const math::Vec3 &cameraPosition
		);

		inline static gfx::IRenderer *s_renderer = nullptr;
		inline static GpuResourceRegistry *s_registry = nullptr;
		inline static AssetManager *s_assetManager = nullptr;
		inline static gfx::GeometryHandle s_fullscreenQuad;
		inline static uint32_t s_fullscreenQuadIndexCount;
	};
} // namespace triple::game

#endif // RENDER_SYSTEM_H

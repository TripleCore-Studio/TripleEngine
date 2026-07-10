#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <entt/entt.hpp>

#include <triple/gfx/IRenderer.h>
#include <triple/gfx/DrawCommand.h>
#include <triple/gfx/FrameArena.h>

#include "triple/game/asset/Primitive.h"

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
		static void submitScene(entt::registry &registry, gfx::ViewHandle view,
		                        gfx::FrameArena &arena, const math::Vec3 &cameraPosition);

	private:
		static void submitPrimitive(const Primitive &prim, gfx::GeometryHandle geometry,
		                            const math::Mat4 &worldMatrix, gfx::ViewHandle view,
		                            gfx::FrameArena &arena, const math::Vec3 &cameraPosition);

		inline static gfx::IRenderer *s_renderer = nullptr;
		inline static GpuResourceRegistry *s_registry = nullptr;
		inline static AssetManager *s_assetManager = nullptr;
	};
} // namespace triple::game

#endif // RENDER_SYSTEM_H

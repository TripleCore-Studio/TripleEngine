#ifndef IRENDERER_H
#define IRENDERER_H

#include "GfxTypes.h"
#include "GfxDescs.h"
#include "DrawCommand.h"

namespace triple::gfx {
	class IRenderer {
	public:
		virtual ~IRenderer() = default;

		// lifecycle
		virtual bool initialize(const RendererConfig &config) = 0;
		virtual void shutdown() = 0;
		virtual void resize(uint32_t width, uint32_t height) = 0;

		// resources
		[[nodiscard]] virtual TextureHandle uploadTexture(const TextureDesc &desc) = 0;
		[[nodiscard]] virtual ShaderHandle uploadShader(const ShaderDesc &desc) = 0;
		[[nodiscard]] virtual GeometryHandle uploadGeometry(const GeometryDesc &desc) = 0;

		virtual void unloadTexture(TextureHandle handle) = 0;
		virtual void unloadShader(ShaderHandle handle) = 0;
		virtual void unloadGeometry(GeometryHandle handle) = 0;

		// views
		[[nodiscard]] virtual ViewHandle createView(const ViewDesc &desc) = 0;
		virtual void destroyView(ViewHandle handle) = 0;
		virtual void updateView(ViewHandle handle, const ViewDesc &desc) = 0;
		[[nodiscard]] virtual RenderTargetHandle getBackBufferTarget() const = 0;

		// frame
		virtual void beginFrame(float time) = 0;
		virtual void submit(ViewHandle view, const DrawCommand &cmd) = 0;
		virtual void endFrame() = 0;
	};

	using CreateRendererFunc = IRenderer *(*)();
	using DestroyRendererFunc = void (*)(IRenderer *);
} // namespace triple::gfx
#endif // IRENDERER_H
#ifndef RENDER_RESOURCE_MANAGER
#define RENDER_RESOURCE_MANAGER

#include <triple/gfx/GfxTypes.h>

#include <unordered_map>

#include "GLGeometry.h"
#include "GLTexture.h"
#include "GLShader.h"

#include <triple/gfx/RenderMaterial.h>
#include <triple/gfx/GeometryDesc.h>
#include <triple/gfx/TextureDesc.h>
#include <triple/gfx/ShaderDesc.h>

namespace triple::gl {
	class RenderResourceManager {
	public:
		GLGeometry* getGLGeometry(gfx::GPUHandle geometry);
		gfx::GPUHandle createGLGeometry(const gfx::GeometryDesc& desc);
		GLTexture* getGLTexture(gfx::GPUHandle texture);
		gfx::GPUHandle createGLTexture(const gfx::TextureDesc& desc);
		GLShader* getGLShader(gfx::GPUHandle shader);
		gfx::GPUHandle createGLShader(const gfx::ShaderDesc& desc);

		void bindMaterial(gfx::RenderMaterial& material);
	private:
		std::unordered_map<gfx::GPUHandle, GLGeometry> m_primitiveCache;
		std::unordered_map<gfx::GPUHandle, GLTexture> m_textureCache;
		std::unordered_map<gfx::GPUHandle, GLShader> m_shaderCache;
		gfx::GPUHandle m_nextHandle = 0;
	};
}

#endif // RENDER_RESOURCE_MANAGER

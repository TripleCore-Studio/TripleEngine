#include "triple/gl/resource/ResourceManager.h"

#include <triple/gfx/GfxTypes.h>

namespace triple::gl {
	struct GLFormatInfo {
		GLenum internalFormat;
		GLenum format;
		GLenum type;
	};

	inline GLFormatInfo toGLFormat(gfx::TextureFormat fmt) {
		switch (fmt) {
			case gfx::TextureFormat::Rgba8:
				return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
			case gfx::TextureFormat::Rgba16F:
				return {GL_RGBA16F, GL_RGBA, GL_FLOAT};
			case gfx::TextureFormat::Depth24Stencil8:
				return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
			default:
				return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
		}
	}

	gfx::TextureHandle ResourceManager::createColorAttachmentTexture(uint16_t width,
	                                                                 uint16_t height,
	                                                                 gfx::TextureFormat format) {

		GLFormatInfo fi = toGLFormat(format);

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, fi.internalFormat, width, height, 0, fi.format, fi.type,
		             nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		GLTextureRes res{tex, GL_TEXTURE_2D, width, height};
		return gfx::TextureHandle{pool<GLTextureRes>(ResourceType::Texture).create(std::move(res))};
	}

	gfx::TextureHandle ResourceManager::createDepthAttachmentTexture(uint16_t width,
	                                                                 uint16_t height,
	                                                                 gfx::TextureFormat format) {

		GLFormatInfo fi = toGLFormat(format);

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, fi.internalFormat, width, height, 0, fi.format, fi.type,
		             nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		GLTextureRes res{tex, GL_TEXTURE_2D, width, height};
		return gfx::TextureHandle{pool<GLTextureRes>(ResourceType::Texture).create(std::move(res))};
	}
} // namespace triple::gl
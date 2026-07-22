#include "triple/gl/utils/TextureUtils.h"

#include <assert.h>

namespace triple::gl {
	GLTextureFormatInfo toGLFormat(gfx::TextureFormat fmt) {
		switch (fmt) {
			case gfx::TextureFormat::Rgba8:
				return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};

			case gfx::TextureFormat::Rgb8:
				return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};

			case gfx::TextureFormat::R8:
				return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};

			case gfx::TextureFormat::Rgba16F:
				return {GL_RGBA16F, GL_RGBA, GL_FLOAT};

			case gfx::TextureFormat::Depth24Stencil8:
				return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
		}

		assert(false && "Unhandled TextureFormat");
		return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
	}
} // namespace triple::gl
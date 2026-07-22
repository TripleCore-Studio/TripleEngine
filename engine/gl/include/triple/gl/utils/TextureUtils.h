#pragma once

#include <glad/glad.h>
#include <triple/gfx/GfxTypes.h>

namespace triple::gl {
	struct GLTextureFormatInfo {
		GLint internalFormat;
		GLenum format;
		GLenum type;
	};

	GLTextureFormatInfo toGLFormat(gfx::TextureFormat fmt);
} // namespace triple::gl
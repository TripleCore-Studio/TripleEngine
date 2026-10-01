#pragma once

#include <cstdint>
#include <vector>

#include <glad/glad.h>

#include <triple/gfx/GfxTypes.h>
#include <triple/gfx/GfxDescs.h>
#include <triple/gfx/DrawCommand.h>

namespace triple::gl {
	enum class ResourceType : uint8_t { Texture, Shader, Geometry, View, RenderTarget, Count };

	struct GLTextureRes {
		GLuint id = 0;
		GLenum target = GL_TEXTURE_2D;
		uint16_t width = 0, height = 0;
	};

	struct GLShaderRes {
		GLuint program = 0;
		uint32_t uniformBlockSize = 0;

		GLint modelLocation = -1;      // "u_model"
		GLint viewLocation = -1;       // "u_view"
		GLint projectionLocation = -1; // "u_projection"

		// TODO(temp): experimental built-ins added for the rain/post-process shader tests, not a
		// designed feature. Remove or replace with a proper per-frame uniform mechanism. To remove,
		// also delete the glGetUniformLocation calls in OpenGLRenderer::uploadShader
		// (OpenGLResource.cpp) and the glUniform1f/glUniform2f in bindShaderIfNeeded
		// (openGLRenderer.cpp).
		GLint timeLocation = -1;       // "uTime" (float, seconds)
		GLint resolutionLocation = -1; // "uResolution" (vec2, view viewport in pixels)
	};

	struct GLGeometryRes {
		GLuint vao = 0, vbo = 0, ebo = 0;
		uint32_t indexCount = 0;
		GLenum indexType = GL_UNSIGNED_INT;
	};

	struct GLRenderTargetRes {
		GLuint fbo = 0;
		std::vector<gfx::TextureHandle> colorTextures;
		gfx::TextureHandle depthTexture;
		uint32_t width = 0, height = 0;
		gfx::RenderTargetType type = gfx::RenderTargetType::BackBuffer;
	};

	struct GLViewRes {
		gfx::ViewDesc desc;
		std::vector<gfx::DrawCommand> queue;
		std::vector<GLenum> activeDrawBuffers;
	};
} // namespace triple::gl
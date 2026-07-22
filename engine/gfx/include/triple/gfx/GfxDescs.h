#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <triple/math/Vec3.h>
#include <triple/math/Mat4.h>

#include "GfxTypes.h"

namespace triple::gfx {
	struct TextureDesc {
		uint16_t width = 0;
		uint16_t height = 0;
		TextureFormat format = TextureFormat::Rgba8;
		const uint8_t *pixels = nullptr;
		bool generateMips = true;
	};

	struct UniformParamDesc {
		std::string name;
		UniformType type = UniformType::Float;
		uint32_t offset = 0; // byte offset inside the packed uniform block
	};

	struct TextureSlotDesc {
		std::string name;
		uint32_t slot = 0; // binding index, matches DrawCommand::textures[slot]
	};

	struct VertexAttributeDesc {
		std::string semantic; // "POSITION", "NORMAL", "TEXCOORD0"...
		VertexAttribType type = VertexAttribType::Float;
		uint32_t location = 0;
		uint32_t offset = 0;
	};

	struct VertexLayout {
		std::vector<VertexAttributeDesc> attributes;
		uint32_t stride = 0; // total size of one vertex in bytes
	};

	struct GeometryDesc {
		const void *vertexData = nullptr;
		uint32_t vertexCount = 0;
		uint32_t vertexStride = 0;
		VertexLayout layout;

		const uint32_t *indices = nullptr;
		uint32_t indexCount = 0;
	};

	struct ShaderDesc {
		std::string vertexSource;
		std::string fragmentSource;

		VertexLayout vertexLayout;
		std::vector<UniformParamDesc> uniforms;
		uint32_t uniformBlockSize = 0; // total size of the packed material blob

		std::vector<TextureSlotDesc> textureSlots;
	};

	enum class RenderTargetType : uint8_t {
		BackBuffer, // swapchain, owned by the renderer
		Texture,    // offscreen, for shadow maps / post-process later
	};

	struct RenderTargetDesc {
		RenderTargetType type = RenderTargetType::BackBuffer;
		uint32_t width = 0;
		uint32_t height = 0;
		TextureFormat colorFormat = TextureFormat::Rgba8;
		bool hasDepth = true;
	};

	struct CameraData {
		math::Mat4 viewMatrix{1.0f};
		math::Mat4 projectionMatrix{1.0f};
		math::Vec3 position{0, 0, 0};
	};

	struct Viewport {
		int32_t x = 0;
		int32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct Color {
		float r = 0.0f;
		float g = 0.0f;
		float b = 0.0f;
		float a = 1.0f;
	};

	struct ViewDesc {
		CameraData camera;
		RenderTargetHandle target;
		Viewport viewport;
		Color clearColor;
		bool clearDepth = true;
	};

	struct RendererConfig {
		void *windowHandle = nullptr;
		uint32_t width = 0;
		uint32_t height = 0;
		bool vsync = true;
	};
} // namespace triple::gfx
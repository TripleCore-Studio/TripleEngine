#pragma once

#include <array>
#include <cstdint>

#include <triple/math/Mat4.h>

#include "GfxTypes.h"
#include "GfxConsts.h"

namespace triple::gfx {
	struct DrawCommand {
		GeometryHandle geometry;
		uint32_t indexOffset = 0;
		uint32_t indexCount = 0;

		ShaderHandle shader;
		std::array<TextureHandle, kMaxTextureSlots> textures{};

		const uint8_t *uniformData = nullptr;
		uint32_t uniformSize = 0;

		math::Mat4 transform{1.0f};

		RenderPass pass = RenderPass::Opaque;
		uint64_t sortKey = 0;
	};
} // namespace triple::gfx
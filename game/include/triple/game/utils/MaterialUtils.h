#pragma once

#include <cstring>

#include <triple/game/asset/Material.h>
#include <triple/gfx/GfxDescs.h>
#include <triple/gfx/DrawCommand.h>
#include <triple/gfx/FrameArena.h>
#include <triple/gfx/UniformAlignment.h>

#include "triple/game/render/GpuResourceRegistry.h"

namespace triple::game {
	class MaterialUtils {
	public:
		inline static void packMaterial(const Material &material, const gfx::ShaderDesc &shaderDesc,
		                                gfx::FrameArena &arena, GpuResourceRegistry &gpuRegistry,
		                                gfx::DrawCommand &outCmd) {
			// --- uniforms ---
			uint8_t *blob = arena.allocate(shaderDesc.uniformBlockSize);

			for (const auto &param : shaderDesc.uniforms) {
				auto it = material.params.find(param.name);

				const float *src = nullptr;
				if (it != material.params.end()) {
					src = it->second.data.data();
				} else {
					// TODO: pull a real default from ShaderDesc once shaders can declare default
					// values; for now missing params are zero-filled.
					static constexpr std::array<float, 16> kZero{};
					src = kZero.data();
				}

				std::memcpy(blob + param.offset, src, sizeOfUniform(param.type));
			}

			outCmd.uniformData = blob;
			outCmd.uniformSize = shaderDesc.uniformBlockSize;

			// --- textures ---
			for (const auto &slot : shaderDesc.textureSlots) {
				auto it = material.textures.find(slot.name);
				if (it == material.textures.end() || slot.slot >= gfx::kMaxTextureSlots) {
					continue; // no texture bound for this slot, leave as invalid handle
				}

				outCmd.textures[slot.slot] =
				    gfx::TextureHandle{gpuRegistry.resolve({AssetType::Texture, it->second.raw})};
			}
		}
	};
} // namespace triple::game
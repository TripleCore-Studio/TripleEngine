#pragma once

#include <triple/gfx/GfxDescs.h>
#include <triple/gfx/DrawCommand.h>
#include <triple/gfx/FrameArena.h>
#include <triple/gfx/UniformAlignment.h>

#include "triple/game/asset/Material.h"
#include "triple/game/asset/MaterialInstance.h"
#include "triple/game/render/GpuResourceRegistry.h"

namespace triple::game {
	class MaterialUtils {
	public:
		static void packMaterialInstance(const MaterialInstance &instance,
		                                 const Material &baseMaterial,
		                                 const gfx::ShaderDesc &shaderDesc, gfx::FrameArena &arena,
		                                 GpuResourceRegistry &gpuRegistry,
		                                 gfx::DrawCommand &outCmd);
	};
} // namespace triple::game
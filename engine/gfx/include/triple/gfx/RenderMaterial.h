#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "GfxTypes.h"
#include "triple/math/Vec4.h"

namespace triple::gfx {
	struct RenderMaterial {
		triple::math::Vec4 albedoColor;
		float metallic;
		float roughness;

		GPUHandle albedoTexHandle;
		GPUHandle normalTexHandle;
		GPUHandle metallicTexHandle;
		GPUHandle roughnessTexHandle;

		GPUHandle shaderHandle;
	};
} // namespace triple::gfx

#endif // RENDER_MATERIAL_H

#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include "Core/CoreTypes.h"
#include "Vec4.h"

namespace TripleEngineCore::Graphics {
	struct RenderMaterial
	{
		TripleMath::Vec4 albedoColor;
		float metallic;
		float roughness;

		GPUHandle albedoTexHandle;
		GPUHandle normalTexHandle;
		GPUHandle metallicTexHandle;
		GPUHandle roughnessTexHandle;

		GPUHandle shaderHandle;
	};
}

#endif // RENDER_MATERIAL_H

#pragma once

#include <cstdint>

#include "GfxTypes.h"

namespace triple::gfx {
	[[nodiscard]] inline uint32_t sizeOfUniform(UniformType type) {
		switch (type) {
			case UniformType::Float:
				return 4;
			case UniformType::Int:
				return 4;
			case UniformType::Vec2:
				return 8;
			case UniformType::Vec3:
				return 12;
			case UniformType::Vec4:
				return 16;
			case UniformType::Mat4:
				return 64;
		}
		return 0;
	}

	[[nodiscard]] inline uint32_t alignmentOfUniform(UniformType type) {
		switch (type) {
			case UniformType::Float:
				return 4;
			case UniformType::Int:
				return 4;
			case UniformType::Vec2:
				return 8;
			case UniformType::Vec3:
				return 16; // vec3 aligns like vec4 in std140!
			case UniformType::Vec4:
				return 16;
			case UniformType::Mat4:
				return 16;
		}
		return 4;
	}

	// TODO: this is GL-style std140 packing. Vulkan (std430) and HLSL cbuffers
	// have different alignment rules — this function will need a backend-specific
	// variant once a second render API is added.
	[[nodiscard]] inline uint32_t alignStd140(UniformType type, uint32_t currentOffset) {
		uint32_t alignment = alignmentOfUniform(type);
		return (currentOffset + alignment - 1) & ~(alignment - 1);
	}
} // namespace triple::gfx
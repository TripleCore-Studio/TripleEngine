#pragma once

#include <unordered_map>
#include <string>

#include <glad/glad.h>

#include <triple/gfx/GfxTypes.h>

namespace triple::gl {
	struct ShaderReflection {
		std::unordered_map<std::string, uint32_t>
		    textureSlots; // texture name -> binding slot (from desc, not GL)
	};
} // namespace triple::gl
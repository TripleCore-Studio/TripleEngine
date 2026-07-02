#pragma once

#include "ILoader.h"
#include "Shader.h"

namespace triple::game {
	class ShaderLoader : public ILoader<Shader> {
	public:
		std::optional<Shader> load(const ShaderLoadParams &params) override;
	};
} // namespace triple::game
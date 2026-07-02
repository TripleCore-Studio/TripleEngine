#pragma once

#include "ILoader.h"
#include "Texture.h"

namespace triple::game {
	class TextureLoader : public ILoader<Texture> {
	public:
		std::optional<Texture> load(const TextureLoadParams &params) override;
	};
} // namespace triple::game
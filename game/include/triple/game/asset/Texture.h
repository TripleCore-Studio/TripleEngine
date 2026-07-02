#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "AssetTypes.h"
#include "LoadersParams.h"

namespace triple::game {
	struct Texture {
		uint16_t width = 0;
		uint16_t height = 0;
		uint8_t channels = 4;
		std::vector<unsigned char> pixels;
	};

	template <>
	struct AssetTypeOf<Texture> {
		static constexpr AssetType kValue = AssetType::Texture;
	};

	template <>
	struct LoadParamsOf<Texture> {
		using Type = TextureLoadParams;
	};
} // namespace triple::game

#endif // TEXTURE_H

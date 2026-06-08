#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "Asset.h"

namespace triple::game {

	struct Texture : public Asset {
		uint16_t width = 0;
		uint16_t height = 0;
		uint8_t channels = 4;
		std::vector<unsigned char> pixels;
	};

} // namespace triple::game

#endif // TEXTURE_H

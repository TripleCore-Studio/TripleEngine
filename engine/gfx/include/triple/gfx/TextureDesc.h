#ifndef TEXTURE_DESC_H
#define TEXTURE_DESC_H

#include <cstdint>

namespace TripleEngineCore::Graphics {
	struct TextureDesc {
		uint16_t width;
		uint16_t height;
		uint8_t channels;
		const unsigned char* data;
	};
}

#endif // TEXTURE_DESC_H

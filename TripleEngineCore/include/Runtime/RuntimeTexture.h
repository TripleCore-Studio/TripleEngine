#ifndef RUNTIME_TEXTURE_H
#define RUNTIME_TEXTURE_H

namespace TripleEngineCore::Runtime {
	struct RuntimeTexture
	{
		RuntimeTexture() : data(nullptr) {}
		RuntimeTexture(uint16_t width, uint16_t height, uint8_t channels, const unsigned char* data, uint64_t contentHash)
			: width(width), height(height), channels(channels), data(data), contentHash(contentHash){
		}
		uint16_t width = 0;
		uint16_t height = 0;
		uint8_t channels = 0;
		const unsigned char* data;

		uint64_t contentHash = 0;
	};
}

#endif // !RUNTIME_TEXTURE_H

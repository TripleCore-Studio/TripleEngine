#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

namespace TripleEngineCore::Graphics {

    struct Texture {
        std::string path;
		uint16_t width;
		uint16_t height;
		std::vector<unsigned char> data;
    };

} // namespace TripleEngineCore::Graphics

#endif // TEXTURE_H

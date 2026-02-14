#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset {

    struct Texture : public Asset {
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t channels = 4;
        std::vector<unsigned char> pixels;
    };

} // namespace TripleEngineCore::Graphics

#endif // TEXTURE_H

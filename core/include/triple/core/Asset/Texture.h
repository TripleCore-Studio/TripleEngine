#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include "Asset.h"

namespace triple::core {

    struct Texture : public Asset {
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t channels = 4;
        std::vector<unsigned char> pixels;
    };

}

#endif // TEXTURE_H

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include "Utils/Hash.h"

namespace TripleEngineCore::Asset {

    struct Texture {
        uint16_t width = 0;
        uint16_t height = 0;
        uint8_t channels = 4;
        std::vector<unsigned char> data;

        uint64_t contentHash = 0;

        void computeHash() {
            uint64_t h = 0;
            h = Hash::combine(h, Hash::fnv1a(&width, sizeof(width)));
            h = Hash::combine(h, Hash::fnv1a(&height, sizeof(height)));
            h = Hash::combine(h, Hash::fnv1a(&channels, sizeof(channels)));
            h = Hash::combine(h, Hash::fnv1a(data.data(), data.size()));
            contentHash = h;
        }
    };

} // namespace TripleEngineCore::Graphics

#endif // TEXTURE_H

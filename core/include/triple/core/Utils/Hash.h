#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <cstddef>

namespace triple::core {

    constexpr uint64_t FNV_OFFSET = 14695981039346656037ull;
    constexpr uint64_t FNV_PRIME = 1099511628211ull;

    inline uint64_t fnv1a(const void* data, size_t size) {
        uint64_t hash = FNV_OFFSET;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);

        for (size_t i = 0; i < size; ++i) {
            hash ^= bytes[i];
            hash *= FNV_PRIME;
        }
        return hash;
    }

    inline uint64_t fnv1a_str(const char* str) {
        uint64_t hash = FNV_OFFSET;
        while (*str) {
            hash ^= static_cast<uint8_t>(*str++);
            hash *= FNV_PRIME;
        }
        return hash;
    }

    inline uint64_t combine(uint64_t a, uint64_t b) {
        return a ^ (b + 0x9e3779b97f4a7c15ull + (a << 6) + (a >> 2));
    }
}

#endif // HASH_H

#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Utils/Hash.h"

namespace TripleEngineCore::Asset {

    struct Shader {
        std::string vertexSource;
        std::string fragmentSource;

        uint64_t vertexHash = 0;
        uint64_t fragmentHash = 0;

        void computeHash() {
            vertexHash = Hash::fnv1a_str(vertexSource.c_str());
            fragmentHash = Hash::fnv1a_str(fragmentSource.c_str());
        }
    };

} // namespace TripleEngineCore::Graphics

#endif // SHADER_H

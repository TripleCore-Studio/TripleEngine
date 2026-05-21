#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset {

    struct Shader : public Asset {
        std::string vertexSource;
        std::string fragmentSource;
    };

} // namespace TripleEngineCore::Graphics

#endif // SHADER_H

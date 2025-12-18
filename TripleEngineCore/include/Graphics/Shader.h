#ifndef SHADER_H
#define SHADER_H

#include <string>

namespace TripleEngineCore::Graphics {

    struct Shader {
        std::string vertexPath;
        std::string fragmentPath;
        std::string vertexSource;
        std::string fragmentSource;
    };

} // namespace TripleEngineCore::Graphics

#endif // SHADER_H

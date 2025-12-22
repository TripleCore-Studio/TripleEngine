#ifndef TEXTURE_GPU_H
#define TEXTURE_GPU_H

#include <cstdint>
#include "Core/CoreTypes.h"

namespace TripleEngineCore::TripleRenderer::Resources
{
    using TextureID = TripleEngineCore::Index;
    using TextureTarget = uint32_t; // GL_TEXTURE_2D, CUBE_MAP...
    using TextureFormat = uint32_t; // GL_RGBA8, GL_DEPTH24_STENCIL8
    using PixelFormat = uint32_t; // GL_RGBA, GL_RGB
    using PixelType = uint32_t; // GL_UNSIGNED_BYTE, GL_FLOAT
    using FilterMode = uint32_t; // GL_LINEAR, GL_NEAREST
    using WrapMode = uint32_t; // GL_REPEAT, GL_CLAMP_TO_EDGE

    inline constexpr TextureID INVALID_TEXTURE_ID = TripleEngineCore::INVALID_INDEX;

    struct TextureGPU
    {
        TextureID     id;        // GPU texture handle
        TextureTarget target;    // texture type (2D, cube...)

        int width;               // width in pixels
        int height;              // height in pixels
        int mipLevels;           // mipmap count

        TextureFormat internalFormat; // GPU storage format
        PixelFormat   format;         // upload format
        PixelType     type;           // upload type

        FilterMode minFilter;    // minification filter
        FilterMode magFilter;    // magnification filter

        WrapMode wrapU;          // U wrap
        WrapMode wrapV;          // V wrap
        WrapMode wrapW;          // W wrap (3D / cube)

        bool hasMipmaps;         // mipmaps generated
    };
}

#endif // TEXTURE_GPU_H

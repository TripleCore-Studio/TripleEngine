#include "Utils/GltfTool.h"

namespace TripleEngineCore::Tool::GLTF {
    bool ReadVec3(const tinygltf::Model& model, int accessorIdx, std::vector<TripleMath::Vec3>& out) {
        if (accessorIdx < 0 || accessorIdx >= model.accessors.size())
            return false;

        const auto& accessor = model.accessors[accessorIdx];
        if (accessor.type != TINYGLTF_TYPE_VEC3)
            return false;

        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];

        const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
        size_t stride = accessor.ByteStride(view) ? accessor.ByteStride(view) : 3 * sizeof(float);

        out.resize(accessor.count);

        for (size_t i = 0; i < accessor.count; ++i) {
            float x = 0, y = 0, z = 0;

            switch (accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                const float* ptr = reinterpret_cast<const float*>(data + i * stride);
                x = ptr[0]; y = ptr[1]; z = ptr[2];
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_BYTE: {
                const int8_t* ptr = reinterpret_cast<const int8_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 127.0f : 1.0f;
                x = ptr[0] * scale;
                y = ptr[1] * scale;
                z = ptr[2] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 255.0f : 1.0f;
                x = ptr[0] * scale;
                y = ptr[1] * scale;
                z = ptr[2] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_SHORT: {
                const int16_t* ptr = reinterpret_cast<const int16_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 32767.0f : 1.0f;
                x = ptr[0] * scale;
                y = ptr[1] * scale;
                z = ptr[2] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 65535.0f : 1.0f;
                x = ptr[0] * scale;
                y = ptr[1] * scale;
                z = ptr[2] * scale;
                break;
            }
            default:
                return false; // unsupported type
            }

            out[i] = { x, y, z };
        }

        return true;
    }

    bool ReadVec2(const tinygltf::Model& model, int accessorIdx, std::vector<TripleMath::Vec2>& out) {
        if (accessorIdx < 0 || accessorIdx >= model.accessors.size())
            return false;

        const auto& accessor = model.accessors[accessorIdx];
        if (accessor.type != TINYGLTF_TYPE_VEC2)
            return false;

        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];

        const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
        size_t stride = accessor.ByteStride(view) ? accessor.ByteStride(view) : 2 * sizeof(float);

        out.resize(accessor.count);

        for (size_t i = 0; i < accessor.count; ++i) {
            float x = 0, y = 0;

            switch (accessor.componentType) {
            case TINYGLTF_COMPONENT_TYPE_FLOAT: {
                const float* ptr = reinterpret_cast<const float*>(data + i * stride);
                x = ptr[0]; y = ptr[1];
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_BYTE: {
                const int8_t* ptr = reinterpret_cast<const int8_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 127.0f : 1.0f;
                x = ptr[0] * scale; y = ptr[1] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 255.0f : 1.0f;
                x = ptr[0] * scale; y = ptr[1] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_SHORT: {
                const int16_t* ptr = reinterpret_cast<const int16_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 32767.0f : 1.0f;
                x = ptr[0] * scale; y = ptr[1] * scale;
                break;
            }
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                const uint16_t* ptr = reinterpret_cast<const uint16_t*>(data + i * stride);
                float scale = accessor.normalized ? 1.0f / 65535.0f : 1.0f;
                x = ptr[0] * scale; y = ptr[1] * scale;
                break;
            }
            default:
                return false; // unsupported type
            }

            out[i] = { x, y };
        }

        return true;
    }

    bool ReadIndices(const tinygltf::Model& model, int accessorIdx, std::vector<uint32_t>& out) {
        if (accessorIdx < 0 || accessorIdx >= model.accessors.size())
            return false;

        const auto& accessor = model.accessors[accessorIdx];
        if (accessor.type != TINYGLTF_TYPE_SCALAR)
            return false;

        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];

        const uint8_t* data = buffer.data.data() + view.byteOffset + accessor.byteOffset;
        size_t stride = accessor.ByteStride(view) ? accessor.ByteStride(view) : 0;

        out.resize(accessor.count);

        switch (accessor.componentType) {
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
            for (size_t i = 0; i < accessor.count; ++i)
                out[i] = reinterpret_cast<const uint8_t*>(data + i * (stride ? stride : sizeof(uint8_t)))[0];
            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
            for (size_t i = 0; i < accessor.count; ++i)
                out[i] = reinterpret_cast<const uint16_t*>(data + i * (stride ? stride : sizeof(uint16_t)))[0];
            break;
        }
        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
            if (!stride || stride == sizeof(uint32_t)) {
                memcpy(out.data(), data, accessor.count * sizeof(uint32_t));
            }
            else {
                for (size_t i = 0; i < accessor.count; ++i)
                    out[i] = reinterpret_cast<const uint32_t*>(data + i * stride)[0];
            }
            break;
        }
        default:
            return false; // unsupported type
        }

        return true;
    }
}
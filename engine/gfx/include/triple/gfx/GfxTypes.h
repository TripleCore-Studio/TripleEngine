#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <cstdint>
#include <limits>

namespace triple::gfx {
	struct GpuHandle {
		uint32_t slot;
		uint32_t generation;

		constexpr bool operator==(const GpuHandle &) const = default;
	};

	inline constexpr GpuHandle kInvalidGpuHandle{(std::numeric_limits<uint32_t>::max)(),
	                                             (std::numeric_limits<uint32_t>::max)()};

	template <typename Tag>
	struct TypedGpuHandle {
		GpuHandle raw = kInvalidGpuHandle;

		constexpr TypedGpuHandle() = default;
		constexpr explicit TypedGpuHandle(GpuHandle handle) : raw(handle) {}

		constexpr bool operator==(const TypedGpuHandle &) const = default;

		[[nodiscard]] constexpr bool isValid() const { return raw != kInvalidGpuHandle; }
	};

	struct TextureTag {};
	struct ShaderTag {};
	struct GeometryTag {};
	struct ViewTag {};
	struct RenderTargetTag {};

	using TextureHandle = TypedGpuHandle<TextureTag>;
	using ShaderHandle = TypedGpuHandle<ShaderTag>;
	using GeometryHandle = TypedGpuHandle<GeometryTag>;
	using ViewHandle = TypedGpuHandle<ViewTag>;
	using RenderTargetHandle = TypedGpuHandle<RenderTargetTag>;

	enum class TextureFormat : uint8_t {
		Rgba8,
		Rgb8,
		R8,
		Rgba16F,
		Depth24Stencil8,
	};

	enum class UniformType : uint8_t {
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat4,
		Int,
	};

	enum class VertexAttribType : uint8_t {
		Float,
		Vec2,
		Vec3,
		Vec4,
	};

	enum class RenderPass : uint8_t {
		Shadow,
		Opaque,
		Transparent,
		PostProcess,
		UI,
		Count,
	};
} // namespace triple::gfx

#endif
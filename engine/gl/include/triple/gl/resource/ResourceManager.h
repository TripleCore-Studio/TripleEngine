#pragma once

#include <array>
#include <memory>

#include "ResourceTypes.h"
#include "ResourcePool.h"

namespace triple::gl {
	template <typename T>
	struct ResourceTraits;

	template <>
	struct ResourceTraits<GLTextureRes> {
		static constexpr ResourceType kType = ResourceType::Texture;
	};
	template <>
	struct ResourceTraits<GLShaderRes> {
		static constexpr ResourceType kType = ResourceType::Shader;
	};
	template <>
	struct ResourceTraits<GLGeometryRes> {
		static constexpr ResourceType kType = ResourceType::Geometry;
	};
	template <>
	struct ResourceTraits<GLViewRes> {
		static constexpr ResourceType kType = ResourceType::View;
	};
	template <>
	struct ResourceTraits<GLRenderTargetRes> {
		static constexpr ResourceType kType = ResourceType::RenderTarget;
	};

	class ResourceManager {
	public:
		ResourceManager() {
			m_pools[idx(ResourceType::Texture)] = std::make_unique<ResourcePool<GLTextureRes>>();
			m_pools[idx(ResourceType::Shader)] = std::make_unique<ResourcePool<GLShaderRes>>();
			m_pools[idx(ResourceType::Geometry)] = std::make_unique<ResourcePool<GLGeometryRes>>();
			m_pools[idx(ResourceType::View)] = std::make_unique<ResourcePool<GLViewRes>>();
			m_pools[idx(ResourceType::RenderTarget)] =
			    std::make_unique<ResourcePool<GLRenderTargetRes>>();
		}

		template <typename T>
		ResourcePool<T> &pool() {
			return static_cast<ResourcePool<T> &>(*m_pools[idx(ResourceTraits<T>::kType)]);
		}

	public:
		gfx::TextureHandle createColorAttachmentTexture(
		    uint16_t width, uint16_t height, gfx::TextureFormat format
		);

		gfx::TextureHandle createDepthAttachmentTexture(
		    uint16_t width, uint16_t height, gfx::TextureFormat format
		);

	private:
		static constexpr size_t idx(ResourceType t) { return static_cast<size_t>(t); }
		std::array<std::unique_ptr<IResourcePool>, static_cast<size_t>(ResourceType::Count)>
		    m_pools;
	};
} // namespace triple::gl

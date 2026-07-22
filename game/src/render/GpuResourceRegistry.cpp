#include "triple/game/render/GpuResourceRegistry.h"

#include <triple/core/event/EventBus.h>
#include <triple/gfx/IRenderer.h>

#include "triple/game/asset/AssetManager.h"

#include "triple/game/asset/Texture.h"
#include "triple/game/asset/Shader.h"
#include <triple/game/asset/Model.h>

namespace triple::game {
	gfx::TextureFormat formatFromChannels(uint8_t channels) {
		switch (channels) {
			case 1:
				return gfx::TextureFormat::R8;
			case 3:
				return gfx::TextureFormat::Rgb8;
			case 4:
				return gfx::TextureFormat::Rgba8;
			default:
				assert(false && "Unsupported channel count");
				return gfx::TextureFormat::Rgba8;
		}
	}

	GpuResourceRegistry::GpuResourceRegistry(gfx::IRenderer *renderer, core::EventBus *bus,
	                                         AssetManager *manager)
	    : m_renderer(renderer), m_bus(bus), m_assetManager(manager) {
		m_bus->addListener<AssetEvent>([this](AssetEvent &e) { onAssetEvent(e); });
	}

	void GpuResourceRegistry::onAssetEvent(AssetEvent &event) {
		switch (event.type()) {
			case AssetType::Texture: {
				AssetEventKind kind = event.kind();
				if (kind == AssetEventKind::Loaded) {
					const Texture *texture =
					    m_assetManager->storageFor<Texture>().get({event.id()});

					assert(texture != nullptr &&
					       "AssetLoaded fired but texture not found in storage");

					gfx::TextureDesc desc{
					    texture->width,
					    texture->height,
					    formatFromChannels(texture->channels),
					    texture->pixels.data(),
					};
					gfx::TextureHandle h = m_renderer->uploadTexture(desc);

					m_handles[GpuResourceKey{AssetType::Texture, event.id()}] = h.raw;
				} else if (kind == AssetEventKind::Reloaded) {
					// TODO: to realize
				} else if (kind == AssetEventKind::Unloaded) {
					// TODO: to realize
				}
				break;
			}
			case AssetType::Shader: {
				AssetEventKind kind = event.kind();
				if (kind == AssetEventKind::Loaded) {
					const Shader *shader = m_assetManager->storageFor<Shader>().get({event.id()});

					assert(shader != nullptr &&
					       "AssetLoaded fired but shader not found in storage");

					gfx::ShaderHandle h = m_renderer->uploadShader(shader->desc);

					m_handles[GpuResourceKey{AssetType::Shader, event.id()}] = h.raw;
				} else if (kind == AssetEventKind::Reloaded) {
					// TODO: to realize
				} else if (kind == AssetEventKind::Unloaded) {
					// TODO: to realize
				}
				break;
			}
			case AssetType::Model: {
				AssetEventKind kind = event.kind();
				if (kind == AssetEventKind::Loaded) {
					const Model *model = m_assetManager->storageFor<Model>().get({event.id()});

					assert(model != nullptr && "AssetLoaded fired but model not found in storage");

					gfx::GeometryDesc desc;
					desc.vertexData = model->vertices.data();
					desc.vertexCount =
					    static_cast<uint32_t>(model->vertices.size() / model->vertexLayout.stride);
					desc.vertexStride = model->vertexLayout.stride;
					desc.layout = model->vertexLayout;
					desc.indices = model->indices.data();
					desc.indexCount = static_cast<uint32_t>(model->indices.size());

					gfx::GeometryHandle h = m_renderer->uploadGeometry(desc);

					m_handles[GpuResourceKey{AssetType::Model, event.id()}] = h.raw;
				} else if (kind == AssetEventKind::Reloaded) {
					// TODO: to realize
				} else if (kind == AssetEventKind::Unloaded) {
					// TODO: to realize
				}
				break;
			}
			default: {
				break;
			}
		}
	}

	gfx::GpuHandle GpuResourceRegistry::resolve(GpuResourceKey key) {
		auto it = m_handles.find(key);
		if (it != m_handles.end())
			return it->second;
		return gfx::kInvalidGpuHandle;
	}
} // namespace triple::game
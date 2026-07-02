#include "triple/game/render/GpuResourceRegistry.h"

#include <triple/core/event/EventBus.h>
#include <triple/gfx/IRenderer.h>

#include "triple/game/asset/AssetManager.h"

#include "triple/game/asset/Texture.h"
#include "triple/game/asset/Shader.h"
#include <triple/game/asset/Model.h>

namespace triple::game {
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

					gfx::TextureDesc desc{texture->width, texture->height, texture->channels,
					                      texture->pixels.data()};
					gfx::GPUHandle h = m_renderer->UploadTexture(desc);

					m_handles[GpuResourceKey{AssetType::Texture, event.id()}] = h;
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

					gfx::ShaderDesc desc{shader->vertexSource.c_str(),
					                     shader->fragmentSource.c_str()};
					gfx::GPUHandle h = m_renderer->UploadShader(desc);

					m_handles[GpuResourceKey{AssetType::Shader, event.id()}] = h;
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

					gfx::GeometryDesc desc{
					    model->vertices.data(), static_cast<uint32_t>(model->vertices.size()),
					    model->indices.data(), static_cast<uint32_t>(model->indices.size())};
					gfx::GPUHandle h = m_renderer->UploadGeometry(desc);

					m_handles[GpuResourceKey{AssetType::Model, event.id()}] = h;
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

	gfx::GPUHandle GpuResourceRegistry::resolve(GpuResourceKey key) {
		auto it = m_handles.find(key);
		if (it != m_handles.end())
			return it->second;
		return gfx::kInvalidGpuHandle;
	}
} // namespace triple::game
#ifndef ASSET_SERVICE_H
#define ASSET_SERVICE_H

#include <array>
#include <memory>
#include <assert.h>

#include <triple/core/event/IEventSink.h>

#include "AssetStorage.h"
#include "ILoader.h"
#include "triple/game/event/AssetEvent.h"

namespace triple::game {
	class AssetManager {
	public:
		AssetManager(core::IEventSink *sink) : m_sink(sink) {}

		template <typename T>
		AssetStorage<T> &storageFor() {
			auto &slot = m_storages[size_t(AssetTypeOf<T>::kValue)];
			if (!slot) {
				slot = std::make_unique<AssetStorage<T>>();
			}
			return *static_cast<AssetStorage<T> *>(slot.get());
		}

		template <typename T>
		ILoader<T> &loaderFor() {
			auto *base = m_loaders[size_t(AssetTypeOf<T>::kValue)].get();
			assert(base && "No loader registered for this asset Type");
			return *static_cast<ILoader<T> *>(base);
		}

		template <typename T>
		TypedAssetID<T> create(std::string name, T asset) {
			auto res = storageFor<T>().insert(std::move(name), std::move(asset));

			if (res.wasReload) {
				auto e = AssetEvent{res.id.raw, AssetTypeOf<T>::kValue, AssetEventKind::Reloaded};
				m_sink->pushEvent(e);
			} else {
				auto e = AssetEvent{res.id.raw, AssetTypeOf<T>::kValue, AssetEventKind::Loaded};
				m_sink->pushEvent(e);
			}

			return res.id;
		}

		template <typename T>
		std::optional<TypedAssetID<T>> load(std::string name,
		                                    const typename LoadParamsOf<T>::Type &params) {
			ILoader<T> &loader = loaderFor<T>();
			std::optional<T> asset = loader.load(params);

			if (!asset) {
				return std::nullopt;
			}

			return create(std::move(name), std::move(*asset));
		}

		// TODO: not cascading, child assets (textures/materials) are not unloaded automatically
		template <typename T>
		void unload(TypedAssetID<T> id) {
			bool removed = storageFor<T>().remove(id.raw);
			if (removed) {
				auto e = AssetEvent{id.raw, AssetTypeOf<T>::kValue, AssetEventKind::Unloaded};
				m_sink->pushEvent(e);
			}
		}

		template <typename T>
		void registerLoader(std::unique_ptr<ILoader<T>> loader) {
			m_loaders[size_t(AssetTypeOf<T>::kValue)] = std::move(loader);
		}

	private:
		std::array<std::unique_ptr<IAssetStorage>, static_cast<size_t>(AssetType::Count)>
		    m_storages;
		std::array<std::unique_ptr<ILoaderBase>, static_cast<size_t>(AssetType::Count)> m_loaders;
		core::IEventSink *m_sink;
	};
} // namespace triple::game

#endif // ASSET_SERVICE_H

#ifndef ASSET_STORAGE_H
#define ASSET_STORAGE_H

#include <memory>
#include <unordered_map>
#include "Core/CoreTypes.h"
#include "Asset/Asset.h"

namespace TripleEngineCore::Asset {
    template<typename T>
    class AssetStorage {
    public:
        AssetID add(const std::string& name, std::unique_ptr<T>&& asset) {
            auto it = m_nameToId.find(name);
            if (it != m_nameToId.end())
                return it->second;

            AssetID id = static_cast<AssetID>(m_assets.size());
            asset->id = id;
            asset->name = name;
            m_assets.push_back(std::move(asset));
            m_nameToId[name] = id;
            return id;
        }

        AssetID getID(const std::string& name) const {
            auto it = m_nameToId.find(name);
            if (it == m_nameToId.end())
                return INVALID_ASSET_ID;
            return it->second;
        }

        const T* get(AssetID id) const {
            if (id == INVALID_ASSET_ID || id >= m_assets.size())
                return nullptr;
            return m_assets[id].get();
        }

        T* getMutable(AssetID id) {
            if (id == INVALID_ASSET_ID || id >= m_assets.size())
                return nullptr;
            return m_assets[id].get();
        }

        bool exists(const std::string& name) const {
            return m_nameToId.find(name) != m_nameToId.end();
        }

        size_t size() const { return m_assets.size(); }

    private:
        std::vector<std::unique_ptr<T>> m_assets;
        std::unordered_map<std::string, AssetID> m_nameToId;
    };
}

#endif // ASSET_STORAGE_H

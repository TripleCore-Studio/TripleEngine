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
            auto it = nameToId.find(name);
            if (it != nameToId.end())
                return it->second;

            AssetID id = static_cast<AssetID>(assets.size());
            asset->id = id;
            asset->name = name;
            assets.push_back(std::move(asset));
            nameToId[name] = id;
            return id;
        }

        AssetID getID(const std::string& name) const {
            auto it = nameToId.find(name);
            if (it == nameToId.end())
                return INVALID_ASSET_ID;
            return it->second;
        }

        const T* get(AssetID id) const {
            if (id == INVALID_ASSET_ID || id >= assets.size())
                return nullptr;
            return assets[id].get();
        }

        T* getMutable(AssetID id) {
            if (id == INVALID_ASSET_ID || id >= assets.size())
                return nullptr;
            return assets[id].get();
        }

        bool exists(const std::string& name) const {
            return nameToId.find(name) != nameToId.end();
        }

        size_t size() const { return assets.size(); }

    private:
        std::vector<std::unique_ptr<T>> assets;
        std::unordered_map<std::string, AssetID> nameToId;
    };
}

#endif // ASSET_STORAGE_H

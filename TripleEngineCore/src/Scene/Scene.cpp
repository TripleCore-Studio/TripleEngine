#include "Scene/Scene.h"

#include <unordered_set>
#include <cstring>

#include "Scene/ParentComponent.h"
#include "Scene/ChildrenComponent.h"
#include "Scene/NameComponent.h"

#include "TLogger.h"
#include "Service/ComponentService.h"

namespace TripleEngineCore::Scene {
    static size_t alignUp(size_t size, size_t align) {
        return (size + align - 1) & ~(align - 1);
    }

    struct ComponentPool {
        ComponentTypeID type = 0;
        size_t stride;
        
        std::vector<Entity> dense;        // index -> entity
        std::vector<size_t> sparse;       // entity -> index
        std::vector<std::byte> data;      // index -> components
    };

    struct Scene::Impl {
        Service::ComponentService* componentService = nullptr;
        std::unordered_map<ComponentTypeID, ComponentPool> pools;
        std::unordered_set<Entity> entities;
        Entity nextEntity = 1;
    };

    Scene::Scene(void* compSrv) : _impl(new Impl{ static_cast<Service::ComponentService*>(compSrv) }) {}
    Scene::~Scene() { delete _impl; }

    Entity Scene::createEntity()
    {
        Entity e = _impl->nextEntity++;
        _impl->entities.insert(e);
        return e;
    }

    Entity Scene::createEntity(const std::string& name) {
        Entity e = _impl->nextEntity++;
        _impl->entities.insert(e);

        if (!name.empty()) {
            auto comp = addComponent<NameComponent>(e);
            if (comp.isValid()) {
                comp->name = name;
            }
        }

        return e;
    }

    void Scene::destroyEntity(Entity e)
    {
        if (_impl->entities.find(e) == _impl->entities.end())
            return;

        _impl->entities.erase(e);

        for (auto& [type, pool] : _impl->pools)
        {
            if (e >= pool.sparse.size())
                continue;

            size_t idx = pool.sparse[e];

            if (idx == SIZE_MAX || idx >= pool.dense.size() || pool.dense[idx] != e)
                continue;

            const auto& info = _impl->componentService->getInfo(type);

            size_t last = pool.dense.size() - 1;

            size_t offset = idx * pool.stride;
            size_t lastOffset = last * pool.stride;

            info.destruct(pool.data.data() + offset);

            if (idx != last)
            {
                if (info.move)
                {
                    info.move(
                        pool.data.data() + offset,
                        pool.data.data() + lastOffset
                    );
                }
                else
                {
                    std::memcpy(
                        pool.data.data() + offset,
                        pool.data.data() + lastOffset,
                        info.size
                    );
                }

                info.destruct(pool.data.data() + lastOffset);

                Entity movedEntity = pool.dense[last];
                pool.dense[idx] = movedEntity;
                pool.sparse[movedEntity] = idx;
            }

            pool.dense.pop_back();
            pool.data.resize(pool.data.size() - pool.stride);

            pool.sparse[e] = SIZE_MAX;
        }
    }

    std::vector<Entity> Scene::getEntities() const
    {
        return std::vector<Entity>(_impl->entities.begin(), _impl->entities.end());
    }

    bool Scene::addChild(Entity parent, Entity child) {
        if (_impl->entities.find(parent) == _impl->entities.end()) return false;
        if (_impl->entities.find(child) == _impl->entities.end()) return false;

        ComponentTypeID parentComponentID = _impl->componentService->getType<ParentComponent>();
        ComponentTypeID childrenComponentID = _impl->componentService->getType<ChildrenComponent>();

        auto existingParent = static_cast<ParentComponent*>(getComponent(child, parentComponentID));
        if (existingParent && existingParent->parent != 0) return false;

        if (addComponent(child, parentComponentID)) {
            auto parentComp = static_cast<ParentComponent*>(getComponent(child, parentComponentID));
            parentComp->parent = parent;
        }

        auto childrenComp = static_cast<ChildrenComponent*>(getComponent(parent, childrenComponentID));
        if (!childrenComp) {
            addComponent(parent, childrenComponentID);
            childrenComp = static_cast<ChildrenComponent*>(getComponent(parent, childrenComponentID));
        }

        childrenComp->children.push_back(child);
        return true;
    }

    void* Scene::addComponent(Entity e, ComponentTypeID type)
    {
        if (_impl->entities.find(e) == _impl->entities.end())
            return nullptr;

        ComponentPool& pool = _impl->pools[type];
        const auto& info = _impl->componentService->getInfo(type);

        if (pool.data.empty()) {
            pool.type = type;
            pool.stride = alignUp(info.size, info.align);
        }

        if (e >= pool.sparse.size())
            pool.sparse.resize(e + 1, SIZE_MAX);

        size_t idx = pool.sparse[e];
        if (idx != SIZE_MAX && idx < pool.dense.size() && pool.dense[idx] == e)
            return pool.data.data() + idx * pool.stride;

        size_t index = pool.dense.size();

        pool.dense.push_back(e);
        pool.sparse[e] = index;

        size_t offset = pool.data.size();
        pool.data.resize(offset + pool.stride);

        info.construct(pool.data.data() + offset);

        return pool.data.data() + offset;
    }

    void* Scene::getComponent(Entity e, ComponentTypeID type)
    {
        auto poolIt = _impl->pools.find(type);
        if (poolIt == _impl->pools.end()) return nullptr;

        ComponentPool& pool = poolIt->second;

        if (e >= pool.sparse.size()) return nullptr;

        size_t idx = pool.sparse[e];
        if (idx == SIZE_MAX || idx >= pool.dense.size() || pool.dense[idx] != e)
            return nullptr;

        return pool.data.data() + idx * pool.stride;
    }

    Scene::ComponentViewRaw Scene::getViewRaw(ComponentTypeID type)
    {
        auto it = _impl->pools.find(type);
        if (it == _impl->pools.end()) {
            return {};
        }

        ComponentPool& pool = it->second;
        return {
            &pool.dense,
            pool.data.data(),
            pool.dense.size(),
            pool.stride
        };
    }

    size_t Scene::getComponentIndex(ComponentTypeID type, Entity e)
    {
        auto it = _impl->pools.find(type);
        if (it == _impl->pools.end()) return SIZE_MAX;

        ComponentPool& pool = it->second;
        if (e >= pool.sparse.size()) return SIZE_MAX;

        size_t idx = pool.sparse[e];
        if (idx == SIZE_MAX || idx >= pool.dense.size() || pool.dense[idx] != e)
            return SIZE_MAX;

        return idx;
    }

    void* Scene::getComponentByIndexChecked(size_t index, ComponentTypeID type, Entity expectedEntity)
    {
        auto it = _impl->pools.find(type);
        if (it == _impl->pools.end()) return nullptr;

        ComponentPool& pool = it->second;

        if (index >= pool.dense.size()) return nullptr;
        if (pool.dense[index] != expectedEntity) return nullptr;

        return pool.data.data() + index * pool.stride;
    }

    uint32_t Scene::getEntityCount() const {
        return static_cast<uint32_t>(_impl->entities.size());
    }

    ComponentTypeID Scene::getComponentTypeID(const std::type_info& type) const
    {
        return _impl->componentService->getTypeByIndex(std::type_index(type));
    }

} // namespace TripleEngineCore::Scene
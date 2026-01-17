#include "Scene/Scene.h"
#include <unordered_set>
#include <cstring>

#include "Scene/ParentComponent.h"
#include "Scene/ChildrenComponent.h"
#include "Scene/NameComponent.h"

namespace TripleEngineCore::Scene {

    struct ComponentPool {
        ComponentTypeID type = 0;
        std::vector<uint8_t> data;
        std::unordered_map<Entity, size_t> entityToIndex;
    };

    struct Scene::Impl {
        ComponentManager* componentManager = nullptr;
        std::unordered_map<ComponentTypeID, ComponentPool> pools;
        std::unordered_set<Entity> entities;
        Entity nextEntity = 1;
    };

    Scene::Scene(ComponentManager* compMgr) : _impl(new Impl{ compMgr }) {}
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
            if (auto nameComp = addComponent<NameComponent>(e)) {
                nameComp->name = name;
            }
        }

        return e;
    }

    void Scene::destroyEntity(Entity e) {
        if (_impl->entities.find(e) == _impl->entities.end()) return;
        _impl->entities.erase(e);

        for (auto& [type, pool] : _impl->pools) {
            auto it = pool.entityToIndex.find(e);
            if (it != pool.entityToIndex.end() && pool.data.size() >= _impl->componentManager->getInfo(type).size) {
                size_t offset = it->second;
                const auto& info = _impl->componentManager->getInfo(type);
                info.destruct(pool.data.data() + offset);

                size_t lastOffset = pool.data.size() - info.size;
                if (offset != lastOffset) {
                    std::memcpy(pool.data.data() + offset, pool.data.data() + lastOffset, info.size);
                    for (auto& [entity, idx] : pool.entityToIndex) {
                        if (idx == lastOffset) { idx = offset; break; }
                    }
                }

                pool.data.resize(pool.data.size() - info.size);
                pool.entityToIndex.erase(it);
            }
        }
    }

    std::vector<Entity> Scene::getEntities() const
    {
        return std::vector<Entity>(_impl->entities.begin(), _impl->entities.end());
    }

    bool Scene::addChild(Entity parent, Entity child) {
        if (_impl->entities.find(parent) == _impl->entities.end()) return false;
        if (_impl->entities.find(child) == _impl->entities.end()) return false;

        ComponentTypeID parentComponentID = _impl->componentManager->getType<TripleEngineCore::Scene::ParentComponent>();
        ComponentTypeID childrenComponentID = _impl->componentManager->getType<TripleEngineCore::Scene::ChildrenComponent>();

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

    void* Scene::addComponent(Entity e, ComponentTypeID type) {
        if (_impl->entities.find(e) == _impl->entities.end()) return nullptr;

        ComponentPool& pool = _impl->pools[type];
        if (pool.data.empty()) pool.type = type;

        const auto& info = _impl->componentManager->getInfo(type);
        size_t offset = pool.data.size();
        pool.data.resize(offset + info.size);
        info.construct(pool.data.data() + offset);
        pool.entityToIndex[e] = offset;

        return pool.data.data() + offset;
    }

    void* Scene::getComponent(Entity e, ComponentTypeID type) {
        auto poolIt = _impl->pools.find(type);
        if (poolIt == _impl->pools.end()) return nullptr;

        ComponentPool& pool = poolIt->second;
        auto it = pool.entityToIndex.find(e);
        if (it == pool.entityToIndex.end()) return nullptr;

        return pool.data.data() + it->second;
    }

    uint32_t Scene::getEntityCount() const {
        return static_cast<uint32_t>(_impl->entities.size());
    }

    ComponentTypeID Scene::getComponentTypeID(const std::type_info& type) const
    {
        return _impl->componentManager->getTypeByIndex(std::type_index(type));
    }

} // namespace TripleEngineCore::Scene
#include "Engine/ComponentManager.h"
#include <stdexcept>

namespace TripleEngineCore {
    Scene::ComponentTypeID ComponentManager::registerComponent(size_t size, size_t align, void(*construct)(void*), void(*destruct)(void*), void(*move)(void*, void*)) {
        if (!construct || !destruct  || !move || size == 0) {
            throw std::invalid_argument("Invalid component registration");
        }

        Scene::ComponentTypeID id = _nextID++;
        _registry[id] = { size, align, construct, destruct, move };
        return id;
    }

    const ComponentInfo& ComponentManager::getInfo(Scene::ComponentTypeID id) const {
        auto it = _registry.find(id);
        if (it == _registry.end()) {
            throw std::runtime_error("ComponentTypeID not registered");
        }
        return it->second;
    }
    Scene::ComponentTypeID ComponentManager::getTypeByIndex(const std::type_index& index) const
    {
        auto it = _typeMap.find(index);
        if (it == _typeMap.end()) {
            return Scene::INVALID_COMPONENT_TYPE_ID;
        }
        return it->second;
    }
}
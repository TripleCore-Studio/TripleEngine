#include "Engine/ComponentManager.h"
#include <stdexcept>

namespace TripleEngineCore {
    ComponentTypeID ComponentManager::registerComponent(size_t size, void(*construct)(void*), void(*destruct)(void*)) {
        if (!construct || !destruct || size == 0) {
            throw std::invalid_argument("Invalid component registration");
        }

        ComponentTypeID id = _nextID++;
        _registry[id] = { size, construct, destruct };
        return id;
    }

    const ComponentInfo& ComponentManager::getInfo(ComponentTypeID id) const {
        auto it = _registry.find(id);
        if (it == _registry.end()) {
            throw std::runtime_error("ComponentTypeID not registered");
        }
        return it->second;
    }
    ComponentTypeID ComponentManager::getTypeByIndex(const std::type_index& index) const
    {
        auto it = _typeMap.find(index);
        if (it == _typeMap.end()) {
            return INVALID_COMPONENT_TYPE_ID;
        }
        return it->second;
    }
}
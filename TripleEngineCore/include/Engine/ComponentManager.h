#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "Core/CoreTypes.h"
#include <unordered_map>
#include <typeindex>

namespace TripleEngineCore {
    using ComponentTypeID = Index;
    inline constexpr ComponentTypeID INVALID_COMPONENT_TYPE_ID = (std::numeric_limits<ComponentTypeID>::max)();

    struct ComponentInfo {
        size_t size;
        void (*construct)(void*);
        void (*destruct)(void*);
    };

    class ComponentManager {
    public:
        template<typename T>
        ComponentTypeID registerComponent() {
            std::type_index ti(typeid(T));

            auto it = _typeMap.find(ti);
            if (it != _typeMap.end())
                return it->second;

            ComponentTypeID id = registerComponent(
                sizeof(T),
                [](void* p) { new (p) T(); },
                [](void* p) { static_cast<T*>(p)->~T(); }
            );

            _typeMap[ti] = id;
            return id;
        }

        template<typename T>
        ComponentTypeID getType() const {
            auto it = _typeMap.find(std::type_index(typeid(T)));
            return it != _typeMap.end() ? it->second : INVALID_COMPONENT_TYPE_ID;
        }

        ComponentTypeID registerComponent(size_t size, void(*construct)(void*), void(*destruct)(void*));
        const ComponentInfo& getInfo(ComponentTypeID id) const;
        ComponentTypeID getTypeByIndex(const std::type_index& index) const;
    private:
        std::unordered_map<ComponentTypeID, ComponentInfo> _registry;
        std::unordered_map<std::type_index, ComponentTypeID> _typeMap;
        ComponentTypeID _nextID = 1;
    };
}

#endif // COMPONENT_MANAGER_H

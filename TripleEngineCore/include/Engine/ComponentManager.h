#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "Core/CoreTypes.h"
#include "Scene/SceneTypes.h"
#include "ExportMacros.h"
#include <unordered_map>
#include <typeindex>
#include <utility>

namespace TripleEngineCore {
    struct ComponentInfo {
        size_t size;
        size_t align;
        void (*construct)(void*);
        void (*destruct)(void*);
        void (*move)(void* dst, void* src);
    };

    class CORE_API_FOR_TESTS ComponentManager {
    public:
        template<typename T>
        Scene::ComponentTypeID registerComponent() {
            std::type_index ti(typeid(T));

            auto it = _typeMap.find(ti);
            if (it != _typeMap.end())
                return it->second;

            Scene::ComponentTypeID id = registerComponent(
                sizeof(T),
                alignof(T),
                [](void* p) { new (p) T(); },
                [](void* p) { static_cast<T*>(p)->~T(); },
                [](void* dst, void* src) noexcept {
                    new (dst) T(std::move(*static_cast<T*>(src)));
                }
            );

            _typeMap[ti] = id;
            return id;
        }

        template<typename T>
        Scene::ComponentTypeID getType() const {
            auto it = _typeMap.find(std::type_index(typeid(T)));
            return it != _typeMap.end() ? it->second : Scene::INVALID_COMPONENT_TYPE_ID;
        }

        Scene::ComponentTypeID registerComponent(size_t size, size_t align, void(*construct)(void*), void(*destruct)(void*), void(*move)(void*, void*));
        const ComponentInfo& getInfo(Scene::ComponentTypeID id) const;
        Scene::ComponentTypeID getTypeByIndex(const std::type_index& index) const;
    private:
        std::unordered_map<Scene::ComponentTypeID, ComponentInfo> _registry;
        std::unordered_map<std::type_index, Scene::ComponentTypeID> _typeMap;
        Scene::ComponentTypeID _nextID = 1;
    };
}

#endif // COMPONENT_MANAGER_H

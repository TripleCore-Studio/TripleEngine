#ifndef COMPONENT_SERVICE_H
#define COMPONENT_SERVICE_H

#include <unordered_map>
#include <typeindex>
#include <utility>

#include "triple/core//CoreTypes.h"
#include "triple/core/Scene/SceneTypes.h"
#include "triple/core/ExportMacros.h"
#include "triple/core/Interfaces/IService.h"

namespace triple::core {
    struct ComponentInfo {
        size_t size;
        size_t align;
        void (*construct)(void*);
        void (*destruct)(void*);
        void (*move)(void* dst, void* src);
    };

    class CORE_API_FOR_TESTS ComponentService : public IService {
    public:
        template<typename T>
        ComponentTypeID registerComponent() {
            std::type_index ti(typeid(T));

            auto it = m_typeMap.find(ti);
            if (it != m_typeMap.end())
                return it->second;

            ComponentTypeID id = registerComponent(
                sizeof(T),
                alignof(T),
                [](void* p) { new (p) T(); },
                [](void* p) { static_cast<T*>(p)->~T(); },
                [](void* dst, void* src) noexcept {
                    new (dst) T(std::move(*static_cast<T*>(src)));
                }
            );

            m_typeMap[ti] = id;
            return id;
        }

        template<typename T>
        ComponentTypeID getType() const {
            auto it = m_typeMap.find(std::type_index(typeid(T)));
            return it != m_typeMap.end() ? it->second : INVALID_COMPONENT_TYPE_ID;
        }

        ComponentTypeID registerComponent(size_t size, size_t align, void(*construct)(void*), void(*destruct)(void*), void(*move)(void*, void*));
        const ComponentInfo& getInfo(ComponentTypeID id) const;
        ComponentTypeID getTypeByIndex(const std::type_index& index) const;
    private:
        std::unordered_map<ComponentTypeID, ComponentInfo> m_registry;
        std::unordered_map<std::type_index, ComponentTypeID> m_typeMap;
        ComponentTypeID m_nextID = 1;
    };
}

#endif // COMPONENT_SERVICE_H

#ifndef COMPONENT_HANDLE_H
#define COMPONENT_HANDLE_H

#include "Scene/SceneTypes.h"
#include <stdint.h>

namespace TripleEngineCore::Scene {
	class Scene;

    template<typename T>
    struct ComponentHandle {
        Scene* scene = nullptr;
        Entity entity = INVALID_ENTITY;
        size_t index = SIZE_MAX;

        ComponentHandle() = default;
        ComponentHandle(Scene* s, Entity e, size_t idx = SIZE_MAX)
            : scene(s), entity(e), index(idx) {
        }

        T* get() const {
            if (!scene) return nullptr;
            return scene->getComponentByIndex<T>(index, entity);
        }

        T& operator*() const { return *get(); }
        T* operator->() const { return get(); }

        bool isValid() const { return get() != nullptr; }
    };

} // namespace TripleEngineCore::Scene

#endif // COMPONENT_HANDLE_H

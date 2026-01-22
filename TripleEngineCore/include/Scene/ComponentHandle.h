#ifndef COMPONENT_HANDLE_H
#define COMPONENT_HANDLE_H

#include "Scene/SceneTypes.h"
#include "Scene/Scene.h"

namespace TripleEngineCore::Scene {

    template<typename T>
    struct ComponentHandle {
        Scene* scene = nullptr;
        Entity entity = INVALID_ENTITY;

        // Constructs a ComponentHandle for the given entity.
        // If 'createIfMissing' is true, the component will be automatically added if it does not exist yet.
        // WARNING: this may create a new component on the entity, so use with care.
        ComponentHandle(Scene* s, Entity e, bool createIfMissing = true)
            : scene(s), entity(e)
        {
            if (createIfMissing && scene && !scene->getComponent<T>(entity)) {
                scene->addComponent<T>(entity);
            }
        }

        // Returns a pointer to the component of type T.
        // WARNING: the pointer may become invalid if new components of the same type are added to the scene.
        // Use the operators * and -> instead of get() for safer access, as they always retrieve the current valid pointer.
        T* get() const {
            return scene ? scene->getComponent<T>(entity) : nullptr;
        }

        T& operator*() const { return *get(); }
        T* operator->() const { return get(); }

        bool isValid() const {
            return scene && entity != INVALID_ENTITY && get() != nullptr;
        }
    };

} // namespace TripleEngineCore::Scene

#endif // COMPONENT_HANDLE_H

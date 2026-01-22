#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <typeinfo>

#include "ExportMacros.h"
#include "Scene/SceneTypes.h"

namespace TripleEngineCore::Scene
{
    class CORE_API Scene {
    public:
        Scene(void* compMgr);
        ~Scene();

        Entity createEntity();
        Entity createEntity(const std::string& name);
        void destroyEntity(Entity e);

        std::vector<Entity> getEntities() const;

        bool addChild(Entity parent, Entity child);

        uint32_t getEntityCount() const;

        // WARNING: The returned pointer is valid only until
        // new components of the same type are added to the scene.
        // After that, the memory may be reallocated and the pointer may become dangling.
        // It is recommended to use `ComponentHandle<T>` instead for safe access.
        template<typename T>
        T* addComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return nullptr;
            return static_cast<T*>(addComponent(e, id));
        }

        // WARNING: The returned pointer is valid only until
        // new components of the same type are added to the scene.
        // After that, the memory may be reallocated and the pointer may become dangling.
        // It is recommended to use `ComponentHandle<T>` instead for safe access.
        template<typename T>
        T* getComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return nullptr;
            return static_cast<T*>(getComponent(e, id));
        }

    private:
        ComponentTypeID getComponentTypeID(const std::type_info& type) const;
        void* addComponent(Entity e, ComponentTypeID type);
        void* getComponent(Entity e, ComponentTypeID type);

        struct Impl;
        Impl* _impl;
    };

} // namespace TripleEngineCore::Scene

#endif // SCENE_H
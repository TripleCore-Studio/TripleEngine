#ifndef SCENE_H
#define SCENE_H

#include "Core/CoreTypes.h"
#include "ExportMacros.h"

#include <string>

#include "Engine/ComponentManager.h"

namespace TripleEngineCore::Scene
{
    using Entity = Index;

    class CORE_API Scene {
    public:
        Scene(ComponentManager* compMgr);
        ~Scene();

        Entity createEntity();
        Entity createEntity(const std::string& name);
        void destroyEntity(Entity e);

        std::vector<Entity> getEntities() const;

        bool addChild(Entity parent, Entity child);

        void* addComponent(Entity e, ComponentTypeID type);
        void* getComponent(Entity e, ComponentTypeID type);

        uint32_t getEntityCount() const;

        template<typename T>
        T* addComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return nullptr;
            return static_cast<T*>(addComponent(e, id));
        }

        template<typename T>
        T* getComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return nullptr;
            return static_cast<T*>(getComponent(e, id));
        }

    private:
        ComponentTypeID getComponentTypeID(const std::type_info& type) const;

        struct Impl;
        Impl* _impl;
    };

} // namespace TripleEngineCore::Scene

#endif // SCENE_H
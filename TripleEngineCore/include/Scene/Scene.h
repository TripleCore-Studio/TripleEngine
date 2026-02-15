#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <typeinfo>

#include "ExportMacros.h"
#include "Scene/SceneTypes.h"
#include "Scene/ComponentHandle.h"

namespace TripleEngineCore::Scene
{
    template<typename T>
    struct ComponentView {
        const std::vector<TripleEngineCore::Scene::Entity>* entities = nullptr;
        std::byte* data = nullptr;
        size_t count = 0;
        size_t stride = 0;

        T& operator[](size_t index) const {
            return *reinterpret_cast<T*>(data + index * stride);
        }
    };

    class CORE_API Scene {
    public:
        Scene(void* compSrv);
        ~Scene();

        Entity createEntity();
        Entity createEntity(const std::string& name);
        void destroyEntity(Entity e);

        std::vector<Entity> getEntities() const;

        bool addChild(Entity parent, Entity child);

        uint32_t getEntityCount() const;

        template<typename T>
        ComponentHandle<T> addComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return {};

            void* ptr = addComponent(e, id);
            if (!ptr) return {};

            size_t index = getComponentIndex(id, e);
            return ComponentHandle<T>(this, e, index);
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

        template<typename T>
        ComponentView<T> getView() {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID)
                return {};

            ComponentViewRaw raw = getViewRaw(id);

            return {
               raw.entities,
               raw.data,
               raw.count,
               raw.stride
            };
        }

        template<typename T>
        T* getComponentByIndex(size_t index, Entity fallbackEntity) {
            if (index == SIZE_MAX) {
                return getComponent<T>(fallbackEntity);
            }
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return nullptr;
            T* component = static_cast<T*>(
                getComponentByIndexChecked(index, id, fallbackEntity)
            );
            if (!component) {
                return getComponent<T>(fallbackEntity);
            }
			return component;
        }

    private:
        struct ComponentViewRaw {
            const std::vector<TripleEngineCore::Scene::Entity>* entities = nullptr;
            std::byte* data = nullptr;
            size_t count = 0;
            size_t stride = 0;
        };

        ComponentTypeID getComponentTypeID(const std::type_info& type) const;
        void* addComponent(Entity e, ComponentTypeID type);
        void* getComponent(Entity e, ComponentTypeID type);
        ComponentViewRaw getViewRaw(ComponentTypeID type);
        size_t getComponentIndex(ComponentTypeID type, Entity e);
        void* getComponentByIndexChecked(size_t index, ComponentTypeID type, Entity expectedEntity);

        struct Impl;
        Impl* _impl;
    };

} // namespace TripleEngineCore::Scene

#endif // SCENE_H
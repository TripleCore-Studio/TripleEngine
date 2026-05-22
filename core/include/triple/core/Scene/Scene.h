#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <typeinfo>

#include "triple/core/ExportMacros.h"
#include "SceneTypes.h"

namespace triple::core
{
    template<typename T>
    struct ComponentView {
        const std::vector<Entity>* entities = nullptr;
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
        T* addComponent(Entity e) {
            ComponentTypeID id = getComponentTypeID(typeid(T));
            if (id == INVALID_COMPONENT_TYPE_ID) return {};

            void* ptr = addComponent(e, id);
            return static_cast<T*>(ptr);
        }

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
            const std::vector<Entity>* entities = nullptr;
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
        Impl* m_impl;
    };

}

#endif // SCENE_H
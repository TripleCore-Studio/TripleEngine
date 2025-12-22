#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <vector>
#include <memory>
#include <type_traits>
#include "Component.h"
#include "Graphics/RenderCommand.h"
#include <string>

namespace TripleEngineCore::Scene {

    class SceneObject {
    public:
        SceneObject(const std::string& name_ = "");
        ~SceneObject() = default;

        uint32_t id;
        std::string name;

        SceneObject* parent = nullptr;
        std::vector<std::unique_ptr<SceneObject>> children;

        std::vector<std::unique_ptr<Component>> components;

        template<typename T, typename... Args>
        T* addComponent(Args&&... args) {
            static_assert(std::is_base_of_v<Component, T>,
                "T must inherit from Component");

            auto comp = std::make_unique<T>(std::forward<Args>(args)...);

            comp->owner = this;
            components.push_back(std::move(comp));

            return nullptr;
        }

        template<typename T>
        T* getComponent() {
            for (auto& comp : components) {
                if (auto casted = dynamic_cast<T*>(comp.get()))
                    return casted;
            }
            return nullptr;
        }

        template<typename T>
        const T* getComponent() const {
            for (auto& comp : components) {
                if (auto casted = dynamic_cast<T*>(comp.get()))
                    return casted;
            }
            return nullptr;
        }

        template<typename T>
        void getAllComponents(std::vector<T*>& outComponents) {
            for (auto& comp : components) {
                if (auto casted = dynamic_cast<T*>(comp.get())) {
                    outComponents.push_back(casted);
                }
            }

            for (auto& child : children) {
                child->getAllComponents<T>(outComponents);
            }
        }

        void addChild(std::unique_ptr<SceneObject> child) {
            child->parent = this;
            children.push_back(std::move(child));
        }

        size_t getSubtreeSize() const;
        size_t getRenderableCount() const;
    private:
        static inline uint32_t nextId = 1;
    };

} // namespace TripleEngineCore::Scene

#endif // SCENE_OBJECT_H

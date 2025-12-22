#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore::Scene {
    SceneObject::SceneObject(const std::string& name_)
        : id(nextId++), name(name_) {
    }
    size_t SceneObject::getSubtreeSize() const
    {
        size_t count = 1;
        for (const auto& child : children) {
            count += child->getSubtreeSize();
        }
        return count;
    }
    size_t SceneObject::getRenderableCount() const
    {
        size_t count = 0;

        if (getComponent<Scene::MeshComponent>() != nullptr) {
            count = 1;
        }

        for (const auto& child : children) {
            count += child->getRenderableCount();
        }

        return count;
    }
}
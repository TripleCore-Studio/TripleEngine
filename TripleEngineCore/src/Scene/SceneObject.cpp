#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore::Scene {
    SceneObject::SceneObject(const std::string& name_)
        : id(nextId++), name(name_) {
    }
}
#include "Scene/SceneObject.h"
#include "Scene/TransformComponent.h"
#include "Scene/MeshComponent.h"

namespace TripleEngineCore::Scene {
    SceneObject::SceneObject() {}

    void SceneObject::update(float dt) {
        for (auto& comp : components) {
            comp->onUpdate(dt);
        }

        for (auto& child : children) {
            child->update(dt);
		}
    }

    void SceneObject::gatherRenderCommands(std::vector<Graphics::RenderCommand>& commands,
        const TripleMath::Mat4& parentWorld) const
    {
        auto transform = getComponent<TransformComponent>();
        TripleMath::Mat4 local = transform ? transform->getModelMatrix() : TripleMath::Mat4::identity();

        TripleMath::Mat4 world = parentWorld * local;

        if (auto meshComp = getComponent<MeshComponent>()) {
            Graphics::RenderCommand cmd;
            cmd.worldMat = world;

            for (auto& mesh : meshComp->model.meshes) {
                cmd.meshes.push_back(&mesh);
            }

            for (auto& mat : meshComp->model.materials) {
                cmd.materials.push_back(&mat);
            }

            commands.push_back(cmd);
        }

        for (auto& child : children) {
            child->gatherRenderCommands(commands, world);
        }
    }
}
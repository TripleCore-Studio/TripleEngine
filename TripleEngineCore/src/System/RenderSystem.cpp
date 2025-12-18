#include "System/RenderSystem.h"
#include "Scene/TransformComponent.h"
#include "Scene/MeshComponent.h"
#include "Mat4.h"
#include "TLogger.h"

#include "Core/CoreTypes.h"

namespace TripleEngineCore::System {
    void RenderSystem::gatherFromObject(const Scene::SceneObject& obj,
        std::vector<Graphics::RenderCommand>& commands,
        const TripleMath::Mat4& parentWorld)
    {
        auto transform = obj.getComponent<Scene::TransformComponent>();
        TripleMath::Mat4 local = transform ? transform->getModelMatrix() : TripleMath::Mat4::identity();
        TripleMath::Mat4 world = parentWorld * local;

        if (auto meshComp = obj.getComponent<Scene::MeshComponent>()) {
            Graphics::RenderCommand cmd;
            cmd.worldMat = world;

            if (meshComp->modelIndex != INVALID_INDEX && assets != nullptr) {
                const auto* model = assets->getModel(meshComp->modelIndex);
                if(model == nullptr) {
                    TripleLogger::TLogger::ModuleWarn("Application",
                        "Model with index {} not found in assets.", std::to_string(meshComp->modelIndex));
					return;
				}

                for (auto& mesh : model->meshes)
                    cmd.meshes.push_back(mesh.get());

                for (auto& mesh : model->meshes) {
                    if (mesh->materialIndex != INVALID_INDEX) {
                        const auto* mat = assets->getMaterial(mesh->materialIndex);
                        if (mat) cmd.materials.push_back(mat);
                    }
                }
            }

            commands.push_back(cmd);
        }

        for (auto& child : obj.children) {
            gatherFromObject(*child, commands, world);
        }
    }

    void RenderSystem::buildRenderCommands(const Scene::Scene& scene,
        std::vector<Graphics::RenderCommand>& commands)
    {
        for (auto& root : scene.rootObjects) {
            gatherFromObject(*root, commands, TripleMath::Mat4::identity());
        }
    }
}

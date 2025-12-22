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

				cmd.items.reserve(model->meshes.size());

                for (const auto& mesh : model->meshes) {
					Graphics::RenderItem item;
					buildRenderItemFromMesh(item, mesh);
					cmd.items.push_back(std::move(item));
                }
            }

            commands.push_back(cmd);
        }

        for (auto& child : obj.children) {
            gatherFromObject(*child, commands, world);
        }
    }

    void RenderSystem::buildRenderItemFromMesh(Graphics::RenderItem& item, const Asset::Mesh& mesh)
    {
        const Asset::Material* meshMaterial = assets->getMaterial(mesh.materialIndex);
        const Asset::Shader* meshShader = meshMaterial ? assets->getShader(meshMaterial->shaderIndex) : nullptr;

        if(meshMaterial == nullptr || meshShader == nullptr) {
            TripleLogger::TLogger::ModuleWarn("Application",
                "Material or Shader not found for mesh during render item build.");
            return;
		}

        auto abtex = assets->getTexture(meshMaterial->albedoTextureIndex);
        Runtime::RuntimeTexture abTexture{ abtex->width, abtex->height, abtex->channels, abtex->data.data(), abtex->contentHash };

        auto normtex = assets->getTexture(meshMaterial->normalTextureIndex);
        Runtime::RuntimeTexture normTexture{ normtex->width, normtex->height, normtex->channels, normtex->data.data(), normtex->contentHash };

        auto metallictex = assets->getTexture(meshMaterial->metallicTextureIndex);
        Runtime::RuntimeTexture metallicTexture{ metallictex->width, metallictex->height, metallictex->channels, metallictex->data.data(), metallictex->contentHash };

        auto roughnesstex = assets->getTexture(meshMaterial->roughnessTextureIndex);
        Runtime::RuntimeTexture roughnessTexture{ roughnesstex->width, roughnesstex->height, roughnesstex->channels, roughnesstex->data.data(), roughnesstex->contentHash };

        Runtime::RuntimeShader runtimeShader{
            meshShader->vertexSource.c_str(),
            meshShader->fragmentSource.c_str(),
            meshShader->vertexHash,
			meshShader->fragmentHash
        };
        Runtime::RuntimeMaterial runtimeMaterial{ meshMaterial->albedoColor,
            meshMaterial->metallic, meshMaterial->roughness,
            abTexture, normTexture, metallicTexture,
            roughnessTexture, runtimeShader
        };

		item.mesh = Runtime::RuntimeMesh{ mesh.vertices.data(), mesh.vertices.size(), mesh.indices.data(), mesh.indices.size(), runtimeMaterial, mesh.geometryHash };
    }

    void RenderSystem::buildRenderCommands(const Scene::Scene& scene,
        std::vector<Graphics::RenderCommand>& commands)
    {
		commands.clear();
		commands.reserve(scene.getRenderableObjectCount());

        for (auto& root : scene.rootObjects) {
            gatherFromObject(*root, commands, TripleMath::Mat4::identity());
        }
    }
}

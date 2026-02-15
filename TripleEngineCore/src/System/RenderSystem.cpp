#include "System/RenderSystem.h"

#include "Scene/TransformComponent.h"
#include "Scene/MeshComponent.h"
#include "Scene/ChildrenComponent.h"
#include "Scene/ParentComponent.h"

#include "Mat4.h"
#include "TLogger.h"
#include "Utils/TransformUtils.h"

#include "Asset/Model.h"
#include "Asset/Material.h"

#include "Graphics/RenderItem.h"

using namespace TripleEngineCore::Graphics;

namespace TripleEngineCore::System {
    void RenderSystem::gatherFromEntity(Scene::Scene* scene,
        Scene::Entity e,
        std::vector<Graphics::RenderCommand>& commands,
        const TripleMath::Mat4& parentWorld)
    {
        auto transform = static_cast<Scene::TransformComponent*>(scene->getComponent<Scene::TransformComponent>(e));
        TripleMath::Mat4 local = transform ? Utils::getModelMatrix(*transform) : TripleMath::Mat4::identity();
        TripleMath::Mat4 world = parentWorld * local;

        if (auto meshComp = static_cast<Scene::MeshComponent*>(scene->getComponent<Scene::MeshComponent>(e))) {
            Graphics::RenderCommand cmd;
            cmd.worldMat = world;

            if (meshComp->modelIndex != INVALID_INDEX && _pAssets != nullptr) {
                const Asset::Model* model = _pAssets->getModel(meshComp->modelIndex);
                buildRenderCmd(cmd, model);
            }

            if (!cmd.items.empty())
                commands.push_back(std::move(cmd));
        }

        if (auto childrenComp = static_cast<Scene::ChildrenComponent*>(scene->getComponent<Scene::ChildrenComponent>(e))) {
            for (Scene::Entity child : childrenComp->children) {
                gatherFromEntity(scene, child, commands, world);
            }
        }
    }

    bool RenderSystem::getGPU(ResourceType type, Asset::AssetID id, GPUHandle& out) {
        switch (type) {
        case ResourceType::Texture:
        {
            auto it = _uploadedTextures.find(id);
            if (it == _uploadedTextures.end()) return false;
            out = it->second;
            return true;
        }
        case ResourceType::Model:
        {
            auto it = _uploadedModels.find(id);
            if (it == _uploadedModels.end()) return false;
            out = it->second;
            return true;
        }
        case ResourceType::Shader:
        {
            auto it = _uploadedShaders.find(id);
            if (it == _uploadedShaders.end()) return false;
            out = it->second;
            return true;
        }
        }
        return false;
    }

    void RenderSystem::buildRenderCmd(Graphics::RenderCommand& cmd, const Asset::Model* obj)
    {
        if (obj) {
            GPUHandle gpuGeometry;
            if (!getGPU(ResourceType::Model, obj->id, gpuGeometry)) return;

            for (auto& mesh : obj->meshes) {
                for (auto& p : mesh.primitives) {
                    RenderItem item;

                    const Asset::Material* mat = _pAssets->getMaterial(p.materialId);
                    if (!mat) mat = _pAssets->getMaterial(_pAssets->getMaterialId(Service::DefaultMaterialName));
                    if (!mat) {
                        TripleLogger::TLogger::ModuleWarn("Core::RenderSystem", "Primitive in mesh({}) skipped", mesh.name);
                        continue;
                    }

                    RenderMaterial rMat;
                    rMat.albedoColor = mat->albedoColor;
                    rMat.metallic = mat->metallic;
                    rMat.roughness = mat->roughness;

                    if (!getGPU(ResourceType::Texture, mat->albedoTextureId, rMat.albedoTexHandle)) continue;
                    if (!getGPU(ResourceType::Texture, mat->metallicTextureId, rMat.metallicTexHandle)) continue;
                    if (!getGPU(ResourceType::Texture, mat->normalTextureId, rMat.normalTexHandle)) continue;
                    if (!getGPU(ResourceType::Texture, mat->roughnessTextureId, rMat.roughnessTexHandle)) continue;
                    if (!getGPU(ResourceType::Shader, mat->shaderId, rMat.shaderHandle)) continue;

                    item.material = rMat;
                    item.geometry = gpuGeometry;
                    item.indexCount = p.indexCount;
                    item.indexOffset = p.indexOffset;

                    cmd.items.push_back(std::move(item));
                }
            }
        }
    }

    void RenderSystem::uploadTexture(const Asset::Texture* texture) {
        if (texture && _pRenderer) {
            auto it = _uploadedTextures.find(texture->id);
            if (it == _uploadedTextures.end()) {
                Graphics::TextureDesc desc;
                desc.width = texture->width;
                desc.height = texture->height;
                desc.channels = texture->channels;
                desc.data = texture->pixels.data();
                GPUHandle h = _pRenderer->UploadTexture(desc);
                _uploadedTextures[texture->id] = h;
            }
        }
    }

    void RenderSystem::uploadGeometry(const Asset::Model* model) {
        auto it = _uploadedModels.find(model->id);
        if (it != _uploadedModels.end()) return;
        Graphics::GeometryDesc desc;
        desc.vertices = model->vertices.data();
        desc.vertexCount = model->vertices.size();
        desc.indices = model->indices.data();
        desc.indexCount = model->indices.size();
        GPUHandle h = _pRenderer->UploadGeometry(desc);
        _uploadedModels[model->id] = h;
    }

    void RenderSystem::uploadShader(const Asset::Shader* shader) {
        if (shader && _pRenderer) {
            auto it = _uploadedShaders.find(shader->id);
            if (it == _uploadedShaders.end()) {
                Graphics::ShaderDesc desc;
                desc.vCode = shader->vertexSource.c_str();
                desc.fCode = shader->fragmentSource.c_str();
                GPUHandle h = _pRenderer->UploadShader(desc);
                _uploadedShaders[shader->id] = h;
            }
        }
    }


    void RenderSystem::buildRenderCommands(Scene::Scene* scene,
        std::vector<Graphics::RenderCommand>& commands)
    {
        commands.clear();
        commands.reserve(scene->getEntityCount());

        for (Scene::Entity e : scene->getEntities()) {
            auto parentComp = static_cast<Scene::ParentComponent*>(scene->getComponent<Scene::ParentComponent>(e));
            if (!parentComp || parentComp->parent == 0) {
                gatherFromEntity(scene, e, commands, TripleMath::Mat4::identity());
            }
        }
    }
}
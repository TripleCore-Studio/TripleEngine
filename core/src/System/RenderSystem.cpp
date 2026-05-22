#include "triple/core/System/RenderSystem.h"

#include "triple/core/Scene/TransformComponent.h"
#include "triple/core/Scene/MeshComponent.h"
#include "triple/core/Scene/ChildrenComponent.h"
#include "triple/core/Scene/ParentComponent.h"

#include <triple/math/Vec4.h>
#include <triple/log/Logger.h>
#include "triple/core/Utils/TransformUtils.h"

#include "triple/core/Asset/Model.h"
#include "triple/core/Asset/Material.h"

#include <triple/gfx/RenderItem.h>

namespace triple::core {
    void RenderSystem::gatherFromEntity(Scene* scene,
        Entity e,
        std::vector<gfx::RenderCommand>& commands,
        const triple::math::Mat4& parentWorld)
    {
        auto transform = static_cast<TransformComponent*>(scene->getComponent<TransformComponent>(e));
        triple::math::Mat4 local = transform ? getModelMatrix(*transform) : triple::math::Mat4::identity();
        triple::math::Mat4 world = parentWorld * local;

        if (auto meshComp = static_cast<MeshComponent*>(scene->getComponent<MeshComponent>(e))) {
            gfx::RenderCommand cmd;
            cmd.worldMat = world;

            if (meshComp->modelIndex != INVALID_INDEX && m_assets != nullptr) {
                const Model* model = m_assets->getModel(meshComp->modelIndex);
                buildRenderCmd(cmd, model);
            }

            if (!cmd.items.empty())
                commands.push_back(std::move(cmd));
        }

        if (auto childrenComp = static_cast<ChildrenComponent*>(scene->getComponent<ChildrenComponent>(e))) {
            for (Entity child : childrenComp->children) {
                gatherFromEntity(scene, child, commands, world);
            }
        }
    }

    bool RenderSystem::getGPU(ResourceType type, AssetID id, gfx::GPUHandle& out) {
        switch (type) {
        case ResourceType::Texture:
        {
            auto it = m_uploadedTextures.find(id);
            if (it == m_uploadedTextures.end()) return false;
            out = it->second;
            return true;
        }
        case ResourceType::Model:
        {
            auto it = m_uploadedModels.find(id);
            if (it == m_uploadedModels.end()) return false;
            out = it->second;
            return true;
        }
        case ResourceType::Shader:
        {
            auto it = m_uploadedShaders.find(id);
            if (it == m_uploadedShaders.end()) return false;
            out = it->second;
            return true;
        }
        }
        return false;
    }

    void RenderSystem::buildRenderCmd(gfx::RenderCommand& cmd, const Model* obj)
    {
        if (obj) {
            gfx::GPUHandle gpuGeometry;
            if (!getGPU(ResourceType::Model, obj->id, gpuGeometry)) return;

            for (auto& mesh : obj->meshes) {
                for (auto& p : mesh.primitives) {
                    gfx::RenderItem item;

                    const Material* mat = m_assets->getMaterial(p.materialId);
                    if (!mat) mat = m_assets->getMaterial(m_assets->getMaterialId(DefaultMaterialName));
                    if (!mat) {
                        triple::log::Logger::ModuleWarn("Core::RenderSystem", "Primitive in mesh({}) skipped", mesh.name);
                        continue;
                    }

                    gfx::RenderMaterial rMat;
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

    void RenderSystem::uploadTexture(const Texture* texture) {
        if (texture && m_renderer) {
            auto it = m_uploadedTextures.find(texture->id);
            if (it == m_uploadedTextures.end()) {
                gfx::TextureDesc desc;
                desc.width = texture->width;
                desc.height = texture->height;
                desc.channels = texture->channels;
                desc.data = texture->pixels.data();
                gfx::GPUHandle h = m_renderer->UploadTexture(desc);
                m_uploadedTextures[texture->id] = h;
            }
        }
    }

    void RenderSystem::uploadGeometry(const Model* model) {
        auto it = m_uploadedModels.find(model->id);
        if (it != m_uploadedModels.end()) return;
        gfx::GeometryDesc desc;
        desc.vertices = model->vertices.data();
        desc.vertexCount = model->vertices.size();
        desc.indices = model->indices.data();
        desc.indexCount = model->indices.size();
        gfx::GPUHandle h = m_renderer->UploadGeometry(desc);
        m_uploadedModels[model->id] = h;
    }

    void RenderSystem::uploadShader(const Shader* shader) {
        if (shader && m_renderer) {
            auto it = m_uploadedShaders.find(shader->id);
            if (it == m_uploadedShaders.end()) {
                gfx::ShaderDesc desc;
                desc.vCode = shader->vertexSource.c_str();
                desc.fCode = shader->fragmentSource.c_str();
                gfx::GPUHandle h = m_renderer->UploadShader(desc);
                m_uploadedShaders[shader->id] = h;
            }
        }
    }


    void RenderSystem::buildRenderCommands(Scene* scene,
        std::vector<gfx::RenderCommand>& commands)
    {
        commands.clear();
        commands.reserve(scene->getEntityCount());

        for (Entity e : scene->getEntities()) {
            auto parentComp = static_cast<ParentComponent*>(scene->getComponent<ParentComponent>(e));
            if (!parentComp || parentComp->parent == 0) {
                gatherFromEntity(scene, e, commands, triple::math::Mat4::identity());
            }
        }
    }
}
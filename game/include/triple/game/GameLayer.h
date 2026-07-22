#pragma once

#include <memory>

#include <triple/core/base/Layer.h>

#include "triple/game/ecs/Scene.h"
#include "triple/game/asset/AssetManager.h"
#include "triple/game/render/GpuResourceRegistry.h"
#include "triple/game/CameraSettings.h"

namespace triple::game {
	class GameLayer : public core::Layer {
	public:
		void onAttach(const core::EngineContext &ctx) override;
		void onDetach() override;
		void onUpdate(float dt) override;
		void onRender(float t) override;
		void onEvent(core::Event &e) override;

		[[nodiscard]] Scene *getActiveScene() const { return m_scene.get(); }
		[[nodiscard]] entt::entity getActiveCamera() const { return m_cameraEntity; }
		[[nodiscard]] AssetManager *getAssetManager() const { return m_assetManager.get(); }

		void setActiveCamera(entt::entity camera) { m_cameraEntity = camera; }

	public:
		CameraSettings cameraSettings;

	private:
		void cameraUpdate(float dt);
		void cameraInit();
		void load();
		void registerDefaultAssets();
		void configureRenderPipeline();

	private:
		std::unique_ptr<AssetManager> m_assetManager;
		std::unique_ptr<GpuResourceRegistry> m_gpuRegistry;

		gfx::ViewHandle m_mainView;

		core::EngineContext m_context;

	private:
		std::unique_ptr<Scene> m_scene;
		entt::entity m_cameraEntity;
	};
} // namespace triple::game
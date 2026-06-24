#pragma once

#include <memory>
#include <functional>

#include <triple/core/base/Layer.h>

#include <triple/gfx/IRenderer.h>

#include "triple/game/ecs/Scene.h"
#include "triple/game/render/RenderSystem.h"
#include "triple/game/asset/AssetService.h"
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
		[[nodiscard]] AssetService *getAssetService() const { return m_assetService.get(); }

		void setActiveCamera(entt::entity camera) { m_cameraEntity = camera; }
		void setFrameContextCallback(std::function<void(gfx::FrameContext &)> cb) {
			m_onFrameContext = cb;
		}

	public:
		CameraSettings cameraSettings;

	private:
		void cameraUpdate(float dt);
		void cameraInit();

	private:
		std::unique_ptr<AssetService> m_assetService;
		std::unique_ptr<RenderSystem> m_renderSystem;
		gfx::IRenderer *m_renderer;
		core::InputSystem *m_inputSystem;

	private:
		std::unique_ptr<Scene> m_scene;
		entt::entity m_cameraEntity;
		std::function<void(gfx::FrameContext &)> m_onFrameContext;
	};
} // namespace triple::game
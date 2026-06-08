#pragma once

#include <memory>
#include <functional>

#include <triple/core/base/Layer.h>

#include <triple/gfx/IRenderer.h>

#include "triple/game/ecs/Scene.h"
#include "triple/game/ecs/ComponentService.h"
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
		[[nodiscard]] Entity getActiveCamera() const { return m_cameraEntity; }
		[[nodiscard]] AssetService *getAssetService() const { return m_assetService.get(); }

		void setActiveCamera(Entity camera) { m_cameraEntity = camera; }
		void setCameraBlock(bool isBlock) { m_isCameraBlocked = isBlock; }
		void setFrameContextCallback(std::function<void(gfx::FrameContext &)> cb) {
			m_onFrameContext = cb;
		}

	public:
		CameraSettings cameraSettings;

	private:
		[[nodiscard]] bool bootstrapComponents() const;
		void cameraUpdate(float dt);
		void cameraInit();

	private:
		std::unique_ptr<AssetService> m_assetService;
		std::unique_ptr<RenderSystem> m_renderSystem;
		std::unique_ptr<ComponentService> m_componentService;
		gfx::IRenderer *m_renderer;
		core::InputSystem *m_inputSystem;

	private:
		std::unique_ptr<Scene> m_scene;
		Entity m_cameraEntity;
		std::function<void(gfx::FrameContext &)> m_onFrameContext;
		bool m_isCameraBlocked = true;
	};
} // namespace triple::game
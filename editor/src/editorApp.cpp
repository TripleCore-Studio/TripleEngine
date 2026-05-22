#include "triple/editor/EditorApp.h"

#include <triple/core/Scene/TransformComponent.h>
#include <triple/core/Scene/CameraComponent.h>
#include <triple/core/Scene/MeshComponent.h>
#include <triple/core/Scene/Scene.h>

#include <triple/log/Logger.h>
#include <triple/math/MathCommon.h>
#include <triple/core/Utils/TransformUtils.h>
#include <triple/core/Event/EngineLoadedEvent.h>

#include <triple/core/System/InputSystem.h>
#include <triple/core/System/InputActionSystem.h>

#include <triple/core/Service/AssetService.h>
#include <triple/core/Service/EventService.h>

using namespace triple::core;
using namespace triple::math;

namespace triple::editor {
	void EditorApp::onUpdate(float dt)
	{
		cameraUpdate(dt);
	}

	void EditorApp::cameraUpdate(float dt)
	{
		TransformComponent* cameraTransform = getActiveScene()->getComponent<TransformComponent>(1);
		if (!cameraTransform) return;

		InputSystem* input = getSystem<InputSystem>();

		Vec2 delta = input->getMouseDelta();
		cameraTransform->rotationEuler.y += -delta.x * _cameraSettings.sensitivity; // yaw
		cameraTransform->rotationEuler.x += delta.y * _cameraSettings.sensitivity; // pitch
		cameraTransform->rotationEuler.x = clamp(cameraTransform->rotationEuler.x, -89.0f, 89.0f);

		Vec3 dir(0, 0, 0);
		if (input->isKeyDown(KeyCode::W)) dir.z += 1;
		if (input->isKeyDown(KeyCode::S)) dir.z -= 1;
		if (input->isKeyDown(KeyCode::A)) dir.x -= 1;
		if (input->isKeyDown(KeyCode::D)) dir.x += 1;
		if (input->isKeyDown(KeyCode::Space)) dir.y += 1;
		if (input->isKeyDown(KeyCode::LeftShift)) dir.y -= 1;

		if (input->isMouseButtonDown(MouseButton::Button5)) {
			_cameraSettings.cameraSpeed += _cameraSettings.cameraSpeedChange * dt;
		}
		else if (input->isMouseButtonDown(MouseButton::Button4)) {
			_cameraSettings.cameraSpeed -= _cameraSettings.cameraSpeedChange * dt;
		}

		_cameraSettings.cameraSpeed = clamp(
			_cameraSettings.cameraSpeed,
			_cameraSettings.cameraSpeedMin,
			_cameraSettings.cameraSpeedMax
		);

		if (dir.length() > 0) {
			dir = dir.normalized() * _cameraSettings.cameraSpeed * dt;

			Vec3 fwd = forward(*cameraTransform);
			Vec3 rightVec = right(*cameraTransform);
			Vec3 up(0, 1, 0);

			cameraTransform->position += fwd * dir.z;
			cameraTransform->position += rightVec * dir.x;
			cameraTransform->position += up * dir.y;
		}
	}

	void EditorApp::demoScene()
	{
		Scene* scene = getActiveScene();

		Entity camera = scene->createEntity("camera");
		auto cameraComp = scene->addComponent<CameraComponent>(camera);
		cameraComp->aspectRatio = 1920.0f / 1080.0f;
		cameraComp->nearPlane = 0.1f;
		cameraComp->farPlane = 100000.0f;
		cameraComp->fov = 70.0f;

		auto transformComp = scene->addComponent<TransformComponent>(camera);
		transformComp->position = Vec3(0, 0, 10);
		transformComp->rotationEuler = Vec3(0, 0, 0);
		transformComp->scale = Vec3(1, 1, 1);

		setActiveCamera(camera);

		AssetService* assets = getService<AssetService>();

		ModelID areaId = assets->loadModelFromFile("area", "assets/models/area.glb");
		ModelID characterId = assets->loadModelFromFile("character", "assets/models/character.glb");

		Entity areaEntity = scene->createEntity();
		auto meshComp = scene->addComponent<MeshComponent>(areaEntity);
		meshComp->modelIndex = areaId;

		auto areaTransform = scene->addComponent<TransformComponent>(areaEntity);
		areaTransform->position = Vec3(0, 0, 0);
		areaTransform->rotationEuler = Vec3(0, 0, 0);
		areaTransform->scale = Vec3(5.0, 5.0, 5.0);

		Entity characterEntity = scene->createEntity();
		auto characterMeshComp = scene->addComponent<MeshComponent>(characterEntity);
		characterMeshComp->modelIndex = characterId;

		auto characterTransform = scene->addComponent<TransformComponent>(characterEntity);
		characterTransform->position = Vec3(7, -0.25, -1.3);
		characterTransform->rotationEuler = Vec3(0, 90, 0);
		characterTransform->scale = Vec3(1.5, 1.5, 1.5);
	}

	void EditorApp::loadCallbacks()
	{
		getService<EventService>()->addListener<EngineLoadedEvent>([this](EngineLoadedEvent& e) {
			demoScene();
		});

		InputTrigger fullscreenTrigger;
		fullscreenTrigger.type = InputTriggerType::Key;
		fullscreenTrigger.state = TriggerState::Pressed;
		fullscreenTrigger.key = KeyCode::F11;

		getSystem<InputActionSystem>()->bind("ToggleFullscreen", {fullscreenTrigger}, [this]() {
			getWindow()->setFullscreen(!getWindow()->isFullscreen());
		});

		InputTrigger captureMouseTrigger;
		captureMouseTrigger.type = InputTriggerType::Key;
		captureMouseTrigger.state = TriggerState::Pressed;
		captureMouseTrigger.key = KeyCode::F10;
		
		getSystem<InputActionSystem>()->bind("ToggleCaptureMouse", { captureMouseTrigger }, [this]() {
			getWindow()->setCursorCapture(!getWindow()->isCursorCaptured());
		});
	}
} // namespace TripleEngineEditor
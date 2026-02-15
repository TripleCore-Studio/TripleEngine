#include "EditorApp.h"

#include <Scene/TransformComponent.h>
#include <Scene/CameraComponent.h>
#include <Scene/MeshComponent.h>
#include <Scene/Scene.h>

#include <TLogger.h>
#include <MathCommon.h>
#include <Utils/TransformUtils.h>
#include <Event/EngineLoadedEvent.h>
#include <cmath>

#include <System/InputSystem.h>
#include <System/InputActionSystem.h>

#include <Service/AssetService.h>
#include <Service/EventService.h>

namespace TEC = TripleEngineCore;
namespace Math = TEC::TripleMath;

using namespace TEC;
using namespace Math;

namespace TripleEngineEditor {
	void EditorApp::onUpdate(float dt)
	{
		cameraUpdate(dt);
	}

	void EditorApp::cameraUpdate(float dt)
	{
		Scene::TransformComponent* cameraTransform = getActiveScene()->getComponent<Scene::TransformComponent>(1);
		if (!cameraTransform) return;

		System::InputSystem* input = getSystem<System::InputSystem>();

		TripleMath::Vec2 delta = input->getMouseDelta();
		cameraTransform->rotationEuler.y += -delta.x * _cameraSettings.sensitivity; // yaw
		cameraTransform->rotationEuler.x += delta.y * _cameraSettings.sensitivity; // pitch
		cameraTransform->rotationEuler.x = TripleMath::clamp(cameraTransform->rotationEuler.x, -89.0f, 89.0f);

		TripleMath::Vec3 dir(0, 0, 0);
		if (input->isKeyDown(Input::KeyCode::W)) dir.z += 1;
		if (input->isKeyDown(Input::KeyCode::S)) dir.z -= 1;
		if (input->isKeyDown(Input::KeyCode::A)) dir.x -= 1;
		if (input->isKeyDown(Input::KeyCode::D)) dir.x += 1;
		if (input->isKeyDown(Input::KeyCode::Space)) dir.y += 1;
		if (input->isKeyDown(Input::KeyCode::LeftShift)) dir.y -= 1;

		if (input->isMouseButtonDown(Input::MouseButton::Button5)) {
			_cameraSettings.cameraSpeed += _cameraSettings.cameraSpeedChange * dt;
		}
		else if (input->isMouseButtonDown(Input::MouseButton::Button4)) {
			_cameraSettings.cameraSpeed -= _cameraSettings.cameraSpeedChange * dt;
		}

		_cameraSettings.cameraSpeed = TripleMath::clamp(
			_cameraSettings.cameraSpeed,
			_cameraSettings.cameraSpeedMin,
			_cameraSettings.cameraSpeedMax
		);

		if (dir.length() > 0) {
			dir = dir.normalized() * _cameraSettings.cameraSpeed * dt;

			TripleMath::Vec3 fwd = Utils::forward(*cameraTransform);
			TripleMath::Vec3 right = Utils::right(*cameraTransform);
			TripleMath::Vec3 up(0, 1, 0);

			cameraTransform->position += fwd * dir.z;
			cameraTransform->position += right * dir.x;
			cameraTransform->position += up * dir.y;
		}
	}

	void EditorApp::demoScene()
	{
		Scene::Scene* scene = getActiveScene();

		Scene::Entity camera = scene->createEntity("camera");
		auto cameraComp = scene->addComponent<Scene::CameraComponent>(camera);
		cameraComp->aspectRatio = 1920.0f / 1080.0f;
		cameraComp->nearPlane = 0.1f;
		cameraComp->farPlane = 100000.0f;
		cameraComp->fov = 70.0f;

		auto transformComp = scene->addComponent<Scene::TransformComponent>(camera);
		transformComp->position = TripleMath::Vec3(0, 0, 10);
		transformComp->rotationEuler = TripleMath::Vec3(0, 0, 0);
		transformComp->scale = TripleMath::Vec3(1, 1, 1);

		setActiveCamera(camera);

		Service::AssetService* assets = getService<Service::AssetService>();
		Service::ModelID modelId = assets->loadModelFromFile("demo_model", "assets/models/demo.glb");

		Scene::Entity entity = scene->createEntity();
		auto meshComp = scene->addComponent<Scene::MeshComponent>(entity);
		meshComp->modelIndex = modelId;

		auto entityTransform = scene->addComponent<Scene::TransformComponent>(entity);
		entityTransform->position = TripleMath::Vec3(0, 0, 0);
		entityTransform->rotationEuler = TripleMath::Vec3(0, 0, 0);
		entityTransform->scale = TripleMath::Vec3(5.0, 5.0, 5.0);
	}

	void EditorApp::loadCallbacks()
	{
		getService<Service::EventService>()->addListener<Event::EngineLoadedEvent>([this](Event::EngineLoadedEvent& e) {
			demoScene();
		});

		Input::InputTrigger fullscreenTrigger;
		fullscreenTrigger.type = Input::InputTriggerType::Key;
		fullscreenTrigger.state = Input::TriggerState::Pressed;
		fullscreenTrigger.key = Input::KeyCode::F11;
		getSystem<System::InputActionSystem>()->bind("ToggleFullscreen", {fullscreenTrigger}, [this]() {
			getWindow()->setFullscreen(!getWindow()->isFullscreen());
		});

		Input::InputTrigger captureMouseTrigger;
		captureMouseTrigger.type = Input::InputTriggerType::Key;
		captureMouseTrigger.state = Input::TriggerState::Pressed;
		captureMouseTrigger.key = Input::KeyCode::F10;
		getSystem<System::InputActionSystem>()->bind("ToggleCaptureMouse", { captureMouseTrigger }, [this]() {
			getWindow()->setCursorCapture(!getWindow()->isCursorCaptured());
		});
	}
} // namespace TripleEngineEditor
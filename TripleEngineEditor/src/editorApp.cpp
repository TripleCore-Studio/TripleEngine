#include "EditorApp.h"

#include <Scene/TransformComponent.h>
#include <Scene/CameraComponent.h>
#include <Scene/MeshComponent.h>

#include "MathCommon.h"
#include "Utils/TransformUtils.h"

using namespace TripleEngineCore;
using namespace TripleMath;

namespace TripleEngineEditor {
	void EditorApp::onUpdate(float dt)
	{
		cameraUpdate(dt);
	}

	void EditorApp::cameraUpdate(float dt)
	{
		Scene::TransformComponent* cameraTransform = getActiveScene()->getComponent<Scene::TransformComponent>(1);
		if (!cameraTransform) return;

		System::InputSystem* input = getInputSys();

		TripleMath::Vec2 delta = input->getMouseDelta();
		cameraTransform->rotationEuler.y += -delta.x * _cameraSettings.sensitivity; // yaw
		cameraTransform->rotationEuler.x += delta.y * _cameraSettings.sensitivity; // pitch
		cameraTransform->rotationEuler.x = TripleMath::clamp(cameraTransform->rotationEuler.x, -89.0f, 89.0f);

		TripleMath::Vec3 dir(0, 0, 0);
		if (input->isKeyDown(Event::KeyCode::W)) dir.z += 1;
		if (input->isKeyDown(Event::KeyCode::S)) dir.z -= 1;
		if (input->isKeyDown(Event::KeyCode::A)) dir.x -= 1;
		if (input->isKeyDown(Event::KeyCode::D)) dir.x += 1;
		if (input->isKeyDown(Event::KeyCode::Space)) dir.y += 1;
		if (input->isKeyDown(Event::KeyCode::LeftShift)) dir.y -= 1;

		if (input->isMouseButtonDown(Event::MouseButton::Button5)) {
			_cameraSettings.cameraSpeed += _cameraSettings.cameraSpeedChange * dt;
		}
		else if (input->isMouseButtonDown(Event::MouseButton::Button4)) {
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
		Scene::Entity camera = getActiveScene()->createEntity("camera");
		Scene::CameraComponent* cameraComp = getActiveScene()->addComponent<Scene::CameraComponent>(camera);
		cameraComp->aspectRatio = 1920.0f / 1080.0f;
		cameraComp->nearPlane = 0.1f;
		cameraComp->farPlane = 1000.0f;
		cameraComp->fov = 70.0f;

		Scene::TransformComponent* transformComp = getActiveScene()->addComponent<Scene::TransformComponent>(camera);
		transformComp->position = TripleMath::Vec3(0, 30, 50);
		transformComp->rotationEuler = TripleMath::Vec3(0, 0, 0);
		transformComp->scale = TripleMath::Vec3(1, 1, 1);

		setActiveCamera(camera);

		System::AssetsSystem* assets = getAssetSys();
		System::ModelID modelId = assets->loadModelFromFile("demo_model", "assets/models/demo.glb");

		Scene::Entity modelEntity = getActiveScene()->createEntity();
		Scene::MeshComponent* meshComp = getActiveScene()->addComponent<Scene::MeshComponent>(modelEntity);
		meshComp->modelIndex = modelId;

		Scene::TransformComponent* modelTransform = getActiveScene()->addComponent<Scene::TransformComponent>(modelEntity);
		modelTransform->position = TripleMath::Vec3(0, 0, 0);
		modelTransform->rotationEuler = TripleMath::Vec3(0, 0, 0);
		modelTransform->scale = TripleMath::Vec3(1, 1, 1);
	}

	void EditorApp::loadCallbacks()
	{
		getEventDispatcher()->addListener(Event::Type::EngineLoaded, [this](Event& e) {
			demoScene();
		});

		System::InputActionSystem::InputTrigger trigger;
		trigger.type = System::InputActionSystem::InputTriggerType::Key;
		trigger.state = System::InputActionSystem::TriggerState::Pressed;
		trigger.key = Event::KeyCode::F11;

		getInputActionSys()->bind("ToggleFullscreen", { trigger }, [this]() {
			getWindow()->toggleFullscreen();
		});
	}
} // namespace TripleEngineEditor
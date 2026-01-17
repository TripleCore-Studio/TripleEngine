#include <iostream>
#include <Engine/Engine.h>

#include <Scene/TransformComponent.h>
#include <Scene/CameraComponent.h>

using namespace TripleEngineCore;
using namespace TripleMath;


class EditorApp : public TripleEngineCore::Engine {
public:
	void onUpdate(float dt) override {
	}

	void demoScene() {
		Scene::Entity camera = getActiveScene()->createEntity("camera");
		Scene::CameraComponent* cameraComp = getActiveScene()->addComponent<Scene::CameraComponent>(camera);
		cameraComp->aspectRatio = 1920.0f / 1080.0f;
		cameraComp->nearPlane = 0.1f;
		cameraComp->farPlane = 1000.0f;
		cameraComp->fov = 70.0f;

		Scene::TransformComponent* transformComp = getActiveScene()->addComponent<Scene::TransformComponent>(camera); 
		transformComp->position = TripleMath::Vec3(0, 0, 0);
		transformComp->rotationEuler = TripleMath::Vec3(0, 0, 0);
		transformComp->scale = TripleMath::Vec3(1, 1, 1);

		setActiveCamera(*cameraComp, *transformComp);
	}

	void initCallbacks() {
		getEventDispatcher()->addListener(Event::Type::EngineLoaded, [this](Event& e) {
			demoScene();
		});
	}
private:
	float _cameraSpeed = 8.0;
};

int main() {
	std::unique_ptr<EditorApp> engine = std::make_unique<EditorApp>();
	engine->initCallbacks();
	Engine::ErrorCode code = engine->start("Triple Editor v(0.0.1)", 800, 600);
	
	std::cout << "Press Enter to exit...";
	std::getchar();
	return static_cast<int>(code);
}
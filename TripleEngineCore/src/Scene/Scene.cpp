#include "Scene/Scene.h"

namespace TripleEngineCore::Scene {
	Scene::Scene()
	{
	}

	void Scene::addRootObject(std::unique_ptr<SceneObject> object)
	{
		rootObjects.push_back(std::move(object));
	}

	void Scene::removeRootObject(SceneObject* object)
	{
		rootObjects.erase(
			std::remove_if(
				rootObjects.begin(),
				rootObjects.end(),
				[object](const std::unique_ptr<SceneObject>& ptr) {
					return ptr.get() == object;
				}
			),
			rootObjects.end()
		);
	}

	void Scene::update(float dt)
	{
		for (auto& obj : rootObjects)
		{
			obj->update(dt);
		}
	}

	void Scene::gatherRenderCommands(std::vector<Graphics::RenderCommand>& commands) const
	{
		for (auto& obj : rootObjects)
		{
			obj->gatherRenderCommands(commands);
		}
	}
}
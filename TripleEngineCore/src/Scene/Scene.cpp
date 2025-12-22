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

	SceneObject* Scene::findObjectById(uint32_t id) {
		for (auto& root : rootObjects) {
			if (auto obj = findInChildrenById(root.get(), id))
				return obj;
		}
		return nullptr;
	}

	std::vector<SceneObject*> Scene::findObjectsByName(const std::string& name) {
		std::vector<SceneObject*> result;
		for (auto& root : rootObjects) {
			findInChildrenByName(root.get(), name, result);
		}
		return result;
	}

	size_t Scene::getTotalObjectCount() const
	{
		size_t count = 0;
		for (const auto& root : rootObjects) {
			count += root->getSubtreeSize();
		}
		return count;
	}

	size_t Scene::getRenderableObjectCount() const
	{
		size_t count = 0;
		for (const auto& root : rootObjects) {
			count += root->getRenderableCount();
		}
		return count;
	}

	SceneObject* Scene::findInChildrenById(SceneObject* obj, uint32_t id) {
		if (obj->id == id) return obj;
		for (auto& child : obj->children) {
			if (auto found = findInChildrenById(child.get(), id))
				return found;
		}
		return nullptr;
	}

	void Scene::findInChildrenByName(SceneObject* obj, const std::string& name, std::vector<SceneObject*>& out) {
		if (obj->name == name) out.push_back(obj);
		for (auto& child : obj->children) {
			findInChildrenByName(child.get(), name, out);
		}
	}
}
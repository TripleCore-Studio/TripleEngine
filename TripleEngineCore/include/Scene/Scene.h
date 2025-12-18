#ifndef SCENE_H
#define SCENE_H

#include "Scene/SceneObject.h"
#include <memory>
#include <vector>

namespace TripleEngineCore::Scene
{
	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		std::vector<std::unique_ptr<SceneObject>> rootObjects;

		template<typename T>
		std::vector<T*> getAllComponents() {
			std::vector<T*> result;
			for (auto& root : rootObjects) {
				root->getAllComponents<T>(result);
			}
			return result;
		}

		void addRootObject(std::unique_ptr<SceneObject> object);

		void removeRootObject(SceneObject* object);

        SceneObject* findObjectById(uint32_t id);

        std::vector<SceneObject*> findObjectsByName(const std::string& name);

    private:
		SceneObject* findInChildrenById(SceneObject* obj, uint32_t id);

		void findInChildrenByName(SceneObject* obj, const std::string& name, std::vector<SceneObject*>& out);
	};
}

#endif // SCENE_H

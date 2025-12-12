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

		void update(float dt);

		void gatherRenderCommands(std::vector<Graphics::RenderCommand>& commands) const;
	};
}

#endif // SCENE_H

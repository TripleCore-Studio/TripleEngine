#ifndef SCENE_TEST_H
#define SCENE_TEST_H

#include <gtest/gtest.h>
#include "Engine/ComponentManager.h"
#include "Scene/Scene.h"
#include "Scene/TransformComponent.h"
#include "Scene/ChildrenComponent.h"
#include "Scene/ParentComponent.h"
#include "Scene/NameComponent.h"

namespace TEC = TripleEngineCore;

namespace TripleEngineTests {
	class SceneTest : public ::testing::Test {
	  protected:
		TEC::ComponentManager cm;
		TEC::Scene::Scene *scene;

		void SetUp() override {
			cm.registerComponent<TEC::Scene::TransformComponent>();
			cm.registerComponent<TEC::Scene::NameComponent>();
			cm.registerComponent<TEC::Scene::ParentComponent>();
			cm.registerComponent<TEC::Scene::ChildrenComponent>();

			scene = new TEC::Scene::Scene(&cm);
		}

		void TearDown() override { delete scene; }
	};
} // namespace TripleEngineTests

#endif // SCENE_TEST_H

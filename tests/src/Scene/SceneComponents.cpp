#include "SceneTest.h"
#include <chrono>

using namespace TEC;
using namespace TEC::Scene;
using namespace TripleEngineTests;

TEST_F(SceneTest, AddAndGetTransformComponent)
{
    Entity e = scene->createEntity();

    auto tr = scene->addComponent<TransformComponent>(e);
    ASSERT_NE(tr.get(), nullptr);

    tr->position = { 1.f, 2.f, 3.f };

    auto* fetched = scene->getComponent<TransformComponent>(e);
    ASSERT_NE(fetched, nullptr);

    EXPECT_FLOAT_EQ(fetched->position.x, 1.f);
    EXPECT_FLOAT_EQ(fetched->position.y, 2.f);
    EXPECT_FLOAT_EQ(fetched->position.z, 3.f);
}

TEST_F(SceneTest, ManyEntitiesTransformIntegrity)
{
    constexpr int N = 5000;
    std::vector<Entity> entities;
    entities.reserve(N);

    for (int i = 0; i < N; ++i) {
        Entity e = scene->createEntity();
        auto tr = scene->addComponent<TransformComponent>(e);
        ASSERT_NE(tr.get(), nullptr);
        tr->position = { float(i), 0.f, 0.f };
        entities.push_back(e);
    }

    for (int i = 0; i < N; ++i) {
        auto* tr = scene->getComponent<TransformComponent>(entities[i]);
        ASSERT_NE(tr, nullptr);
        EXPECT_FLOAT_EQ(tr->position.x, float(i));
    }
}

TEST_F(SceneTest, DestroyEntityKeepsOtherComponentsValid)
{
    Entity e1 = scene->createEntity();
    Entity e2 = scene->createEntity();

    scene->addComponent<NameComponent>(e1)->name = "A";
    scene->addComponent<NameComponent>(e2)->name = "B";

    scene->destroyEntity(e1);

    EXPECT_EQ(scene->getComponent<NameComponent>(e1), nullptr);

    auto* name2 = scene->getComponent<NameComponent>(e2);
    ASSERT_NE(name2, nullptr);
    EXPECT_EQ(name2->name, "B");
}

TEST_F(SceneTest, ViewReturnsCorrectEntitiesAndComponents)
{
    Entity e1 = scene->createEntity();
    Entity e2 = scene->createEntity();

    auto tr1 = scene->addComponent<TransformComponent>(e1);
    auto tr2 = scene->addComponent<TransformComponent>(e2);

    tr1->position = { 1.f, 1.f, 1.f };
    tr2->position = { 2.f, 2.f, 2.f };

    auto view = scene->getView<TransformComponent>();
    ASSERT_NE(view.entities, nullptr);
    EXPECT_EQ(view.count, 2);

    std::unordered_map<Entity, TripleMath::Vec3> expected = {
        { e1, {1.f, 1.f, 1.f} },
        { e2, {2.f, 2.f, 2.f} }
    };

    for (size_t i = 0; i < view.count; ++i) {
        Entity e = (*view.entities)[i];
        TransformComponent& t = view[i];

        auto it = expected.find(e);
        ASSERT_NE(it, expected.end()) << "Unexpected entity in view";

        const TripleMath::Vec3& pos = it->second;
        EXPECT_FLOAT_EQ(t.position.x, pos.x);
        EXPECT_FLOAT_EQ(t.position.y, pos.y);
        EXPECT_FLOAT_EQ(t.position.z, pos.z);

        expected.erase(it);
    }

    EXPECT_TRUE(expected.empty()) << "Some entities were missing from the view";
}

TEST_F(SceneTest, DestroyEntityUpdatesSparseAndDense)
{
    Entity e1 = scene->createEntity();
    Entity e2 = scene->createEntity();
    Entity e3 = scene->createEntity();

    scene->addComponent<TransformComponent>(e1);
    scene->addComponent<TransformComponent>(e2);
    scene->addComponent<TransformComponent>(e3);

    scene->destroyEntity(e2);

    auto view = scene->getView<TransformComponent>();
    ASSERT_EQ(view.count, 2);

    for (size_t i = 0; i < view.count; ++i) {
        Entity e = (*view.entities)[i];
        EXPECT_NE(e, e2);
        auto* comp = scene->getComponent<TransformComponent>(e);
        ASSERT_NE(comp, nullptr);
    }
}

TEST_F(SceneTest, MultipleComponentTypesIndependence)
{
    Entity e = scene->createEntity();

    scene->addComponent<TransformComponent>(e)->position = { 1,2,3 };
    scene->addComponent<NameComponent>(e)->name = "Entity";

    auto* tr = scene->getComponent<TransformComponent>(e);
    auto* name = scene->getComponent<NameComponent>(e);

    ASSERT_NE(tr, nullptr);
    ASSERT_NE(name, nullptr);

    EXPECT_EQ(name->name, "Entity");
    EXPECT_FLOAT_EQ(tr->position.x, 1.f);
}

TEST_F(SceneTest, CreateEntityWithName)
{
    Entity e = scene->createEntity("TestEntity");

    auto* nameComp = scene->getComponent<NameComponent>(e);
    ASSERT_NE(nameComp, nullptr);
    EXPECT_EQ(nameComp->name, "TestEntity");
}
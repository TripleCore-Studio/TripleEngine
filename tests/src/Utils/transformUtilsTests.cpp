#include <gtest/gtest.h>
#include <Utils/TransformUtils.h>
#include <MathCommon.h>

using namespace TripleEngineCore;
using namespace TripleEngineCore::TripleMath;
using namespace TripleEngineCore::Utils;

static bool nearlyEqual(const Vec3 &a, const Vec3 &b, float eps = 1e-4f) {
	return std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps;
}

//
// Orthonormal basis
//
TEST(TransformUtils, BasisOrthogonal) {
	Scene::TransformComponent t{};
	t.rotationEuler = {13, 47, 29};

	Vec3 f = forward(t);
	Vec3 r = right(t);
	Vec3 u = up(t);

	EXPECT_NEAR(dot(f, r), 0.0f, 1e-4f);
	EXPECT_NEAR(dot(f, u), 0.0f, 1e-4f);
	EXPECT_NEAR(dot(r, u), 0.0f, 1e-4f);
}

TEST(TransformUtils, BasisNormalized) {
	Scene::TransformComponent t{};
	t.rotationEuler = {17, 11, 93};

	EXPECT_NEAR(forward(t).length(), 1.0f, 1e-4f);
	EXPECT_NEAR(right(t).length(), 1.0f, 1e-4f);
	EXPECT_NEAR(up(t).length(), 1.0f, 1e-4f);
}

//
// Cross product consistency
//
TEST(TransformUtils, CrossProducts) {
	Scene::TransformComponent t{};
	t.rotationEuler = {23, 77, 12};

	Vec3 f = forward(t);
	Vec3 r = right(t);
	Vec3 u = up(t);

	EXPECT_TRUE(nearlyEqual(normalize(cross(r, u)), f));
}

//
// Rotation matrix consistency
//
TEST(TransformUtils, ForwardMatchesRotationMatrix) {
	Scene::TransformComponent t{};
	t.rotationEuler = {12, 45, 3};

	Mat4 R = getRotationMatrix(t);
	Vec4 vf = R * Vec4(0, 0, -1, 0);
	Vec3 expected = normalize(Vec3(vf.x, vf.y, vf.z));

	EXPECT_TRUE(nearlyEqual(forward(t), expected));
}

//
// Model matrix translation
//
TEST(TransformUtils, ModelMatrixTranslation) {
	Scene::TransformComponent t{};
	t.position = {5, 7, -3};

	Mat4 M = getModelMatrix(t);
	Vec4 p = M * Vec4(0, 0, 0, 1);

	EXPECT_NEAR(p.x, 5.0f, 1e-4f);
	EXPECT_NEAR(p.y, 7.0f, 1e-4f);
	EXPECT_NEAR(p.z, -3.0f, 1e-4f);
}

//
// Model matrix scale
//
TEST(TransformUtils, ModelMatrixScale) {
	Scene::TransformComponent t{};
	t.scale = {2, 3, 4};

	Mat4 M = getModelMatrix(t);

	Vec4 px = M * Vec4(1, 0, 0, 0);
	Vec4 py = M * Vec4(0, 1, 0, 0);
	Vec4 pz = M * Vec4(0, 0, 1, 0);

	EXPECT_NEAR(Vec3(px.x, px.y, px.z).length(), 2.0f, 1e-4f);
	EXPECT_NEAR(Vec3(py.x, py.y, py.z).length(), 3.0f, 1e-4f);
	EXPECT_NEAR(Vec3(pz.x, pz.y, pz.z).length(), 4.0f, 1e-4f);
}
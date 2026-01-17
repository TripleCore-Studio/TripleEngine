#include "Utils/TransformUtils.h"
#include "Mat4Operations.h"
#include "MathCommon.h"

using namespace TripleEngineCore::TripleMath;

constexpr float DEG2RAD = 3.14159265359f / 180.0f;

namespace TripleEngineCore::Utils {
	TripleMath::Mat4 getModelMatrix(const Scene::TransformComponent& t)
	{
        float rx = t.rotationEuler.x * DEG2RAD;
        float ry = t.rotationEuler.y * DEG2RAD;
        float rz = t.rotationEuler.z * DEG2RAD;

        Mat4 rotX = rotate(rx, { 1, 0, 0 });
        Mat4 rotY = rotate(ry, { 0, 1, 0 });
        Mat4 rotZ = rotate(rz, { 0, 0, 1 });
        Mat4 tr = translate(t.position);
        Mat4 s = TripleMath::scale(t.scale);

        // M = T * Rz * Ry * Rx * S
        return multiply(tr, multiply(rotZ, multiply(rotY, multiply(rotX, s))));
	}
    TripleMath::Mat4 getRotationMatrix(const Scene::TransformComponent& t)
    {
        float rx = t.rotationEuler.x * DEG2RAD;
        float ry = t.rotationEuler.y * DEG2RAD;
        float rz = t.rotationEuler.z * DEG2RAD;

        Mat4 Rx = rotate(rx, { 1,0,0 });
        Mat4 Ry = rotate(ry, { 0,1,0 });
        Mat4 Rz = rotate(rz, { 0,0,1 });

        return multiply(Rz, multiply(Ry, Rx));
    }
    TripleMath::Vec3 forward(const Scene::TransformComponent& t)
    {
        Mat4 R = getRotationMatrix(t);
        Vec4 v = R * Vec4(0, 0, -1, 0);
        return normalize(Vec3(v.x, v.y, v.z));
    }
    TripleMath::Vec3 right(const Scene::TransformComponent& t)
    {
        Mat4 R = getRotationMatrix(t);
        Vec4 v = R * Vec4(1, 0, 0, 0);
        return normalize(Vec3(v.x, v.y, v.z));
    }
    TripleMath::Vec3 up(const Scene::TransformComponent& t)
    {
        Mat4 R = getRotationMatrix(t);
        Vec4 v = R * Vec4(0, 1, 0, 0);
        return normalize(Vec3(v.x, v.y, v.z));
    }
}

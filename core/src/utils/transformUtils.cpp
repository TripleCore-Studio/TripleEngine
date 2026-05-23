#include "triple/core/Utils/TransformUtils.h"
#include <triple/math/Mat4Operations.h>
#include <triple/math/MathCommon.h>

constexpr float DEG2RAD = 3.14159265359f / 180.0f;

using namespace triple::math;

namespace triple::core {
	triple::math::Mat4 getModelMatrix(const TransformComponent &t) {
		float rx = t.rotationEuler.x * DEG2RAD;
		float ry = t.rotationEuler.y * DEG2RAD;
		float rz = t.rotationEuler.z * DEG2RAD;

		Mat4 rotX = rotate(rx, {1, 0, 0});
		Mat4 rotY = rotate(ry, {0, 1, 0});
		Mat4 rotZ = rotate(rz, {0, 0, 1});
		Mat4 tr = translate(t.position);
		Mat4 s = triple::math::scale(t.scale);

		// M = T * Rz * Ry * Rx * S
		return tr * rotZ * rotY * rotX * s;
	}
	triple::math::Mat4 getRotationMatrix(const TransformComponent &t) {
		float rx = t.rotationEuler.x * DEG2RAD;
		float ry = t.rotationEuler.y * DEG2RAD;
		float rz = t.rotationEuler.z * DEG2RAD;

		Mat4 Rx = rotate(rx, {1, 0, 0});
		Mat4 Ry = rotate(ry, {0, 1, 0});
		Mat4 Rz = rotate(rz, {0, 0, 1});

		return Rz * Ry * Rx;
	}
	triple::math::Vec3 forward(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(0, 0, -1, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
	triple::math::Vec3 right(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(1, 0, 0, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
	triple::math::Vec3 up(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(0, 1, 0, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
} // namespace triple::core
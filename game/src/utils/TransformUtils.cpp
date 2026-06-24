#include "triple/game/utils/TransformUtils.h"

#include <triple/math/Mat4Operations.h>
#include <triple/math/MathCommon.h>

using namespace triple::math;

namespace triple::game {
	triple::math::Mat4 TransformUtils::getModelMatrix(const TransformComponent &t) {
		float rx = t.rotationEuler.x * DEG2RAD;
		float ry = t.rotationEuler.y * DEG2RAD;
		float rz = t.rotationEuler.z * DEG2RAD;

		Mat4 rotX = Mat4Operations::rotate(rx, {1, 0, 0});
		Mat4 rotY = Mat4Operations::rotate(ry, {0, 1, 0});
		Mat4 rotZ = Mat4Operations::rotate(rz, {0, 0, 1});
		Mat4 tr = Mat4Operations::translate(t.position);
		Mat4 s = Mat4Operations::scale(t.scale);

		// M = T * Rz * Ry * Rx * S
		return tr * rotZ * rotY * rotX * s;
	}
	triple::math::Mat4 TransformUtils::getRotationMatrix(const TransformComponent &t) {
		float rx = t.rotationEuler.x * DEG2RAD;
		float ry = t.rotationEuler.y * DEG2RAD;
		float rz = t.rotationEuler.z * DEG2RAD;

		Mat4 Rx = Mat4Operations::rotate(rx, {1, 0, 0});
		Mat4 Ry = Mat4Operations::rotate(ry, {0, 1, 0});
		Mat4 Rz = Mat4Operations::rotate(rz, {0, 0, 1});

		return Rz * Ry * Rx;
	}
	triple::math::Vec3 TransformUtils::forward(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(0, 0, -1, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
	triple::math::Vec3 TransformUtils::right(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(1, 0, 0, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
	triple::math::Vec3 TransformUtils::up(const TransformComponent &t) {
		Mat4 R = getRotationMatrix(t);
		Vec4 v = R * Vec4(0, 1, 0, 0);
		return normalize(Vec3(v.x, v.y, v.z));
	}
} // namespace triple::game
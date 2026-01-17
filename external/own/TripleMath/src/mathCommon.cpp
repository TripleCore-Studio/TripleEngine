#include "MathCommon.h"
#include <cmath>

namespace TripleEngineCore::TripleMath {
	Vec3 normalize(const Vec3& v)
	{
		float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if (len == 0.0f) return Vec3{ 0.0f, 0.0f, 0.0f };
		return Vec3{ v.x / len, v.y / len, v.z / len };
	}
}
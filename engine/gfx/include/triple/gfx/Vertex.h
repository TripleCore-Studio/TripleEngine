#ifndef VERTEX_H
#define VERTEX_H

#include "triple/math/Vec2.h"
#include "triple/math/Vec3.h"

namespace triple::gfx {
	struct Vertex {
		triple::math::Vec3 position;
		triple::math::Vec3 normal;
		triple::math::Vec3 tangent;
		triple::math::Vec3 bitangent;
		triple::math::Vec3 color;
		triple::math::Vec2 uv;
	};
} // namespace triple::gfx
#endif // VERTEX_H

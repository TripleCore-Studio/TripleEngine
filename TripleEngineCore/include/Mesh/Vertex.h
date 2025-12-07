#ifndef VERTEX_H
#define VERTEX_H

#include "Vec2.h"
#include "Vec3.h"

namespace TripleEngineCore::Graphics {
	struct Vertex
	{
		TripleMath::Vec3 position;
		TripleMath::Vec3 normal;
		TripleMath::Vec3 tangent;
		TripleMath::Vec3 bitangent;
		TripleMath::Vec3 color;
		TripleMath::Vec2 uv;
	};
}
#endif // VERTEX_H

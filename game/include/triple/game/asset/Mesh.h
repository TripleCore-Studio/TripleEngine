#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include "Primitive.h"

namespace triple::game {
	struct Mesh {
	public:
		std::string name;
		std::vector<Primitive> primitives;
	};
} // namespace triple::game

#endif // MESH_H

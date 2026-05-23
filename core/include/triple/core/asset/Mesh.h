#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "Primitive.h"

namespace triple::core {
	struct Mesh {
	  public:
		std::string name;
		std::vector<Primitive> primitives;
	};
} // namespace triple::core

#endif // MESH_H

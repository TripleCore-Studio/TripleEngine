#ifndef RUNTIME_MODEL_H
#define RUNTIME_MODEL_H

#include <vector>
#include "RuntimeMesh.h"

namespace TripleEngineCore::Runtime {
	struct RuntimeModel
	{
	public:
		RuntimeModel() {}
		std::vector<RuntimeMesh> meshes;
	};
}

#endif // RUNTIME_MODEL_H

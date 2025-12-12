#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include <vector>
#include "Mesh.h"
#include "Material.h"
#include "Mat4.h"

namespace TripleEngineCore::Graphics {
	struct RenderCommand
	{
		TripleMath::Mat4 worldMat;
		std::vector<const Mesh*> meshes;
		std::vector<const Material*> materials;
	};
}

#endif // RENDER_COMMAND_H
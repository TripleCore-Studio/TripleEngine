#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include <vector>
#include "Mat4.h"
#include "RenderItem.h"

namespace TripleEngineCore::Graphics {
	struct RenderCommand
	{
		TripleMath::Mat4 worldMat;
		std::vector<RenderItem> items;
	};
}

#endif // RENDER_COMMAND_H
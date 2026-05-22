#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "triple/math/Mat4.h"
#include "RenderItem.h"
#include <vector>

namespace triple::gfx {
	struct RenderCommand
	{
		triple::math::Mat4 worldMat;
		std::vector<RenderItem> items;
	};
}

#endif // RENDER_COMMAND_H
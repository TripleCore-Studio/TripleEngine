#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include "triple/math/Mat4.h"
#include "triple/math/Vec3.h"

namespace triple::gfx
{
	struct CameraData { triple::math::Mat4 view; triple::math::Mat4 proj; triple::math::Vec3 pos;};
}

#endif // CAMERA_DATA_H

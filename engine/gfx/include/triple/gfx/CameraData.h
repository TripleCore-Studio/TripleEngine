#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include "Mat4.h"
#include "Vec3.h"

namespace TripleEngineCore::Graphics
{
	struct CameraData { TripleMath::Mat4 view; TripleMath::Mat4 proj; TripleMath::Vec3 pos;};
}

#endif // CAMERA_DATA_H

#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include "Mat4.h"
namespace TripleEngineCore
{
	struct CameraData { TripleMath::Mat4 view; TripleMath::Mat4 proj; };
}

#endif // CAMERA_DATA_H

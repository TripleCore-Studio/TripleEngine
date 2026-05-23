#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Asset.h"

namespace triple::core {

	struct Shader : public Asset {
		std::string vertexSource;
		std::string fragmentSource;
	};

} // namespace triple::core

#endif // SHADER_H

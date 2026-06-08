#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Asset.h"

namespace triple::game {

	struct Shader : public Asset {
		std::string vertexSource;
		std::string fragmentSource;
	};

} // namespace triple::game

#endif // SHADER_H

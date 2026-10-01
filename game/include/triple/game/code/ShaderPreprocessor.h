#pragma once

#include <string>

namespace triple::game {
	class ShaderPreprocessor {
	public:
		ShaderPreprocessor();

	public:
		std::string process(const std::string &code);

	private:
	};
} // namespace triple::game

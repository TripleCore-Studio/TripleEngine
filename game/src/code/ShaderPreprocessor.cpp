#include "triple/game/code/ShaderPreprocessor.h"

#include "triple/log/Logger.h"

#include <filesystem>
#include <fstream>

namespace triple::game {
	std::optional<std::string> ShaderPreprocessor::process(std::string_view source) {
		std::optional<std::string> result = std::string();
		result->reserve(source.size());

		constexpr std::string_view kPragma = "#pragma ";

		size_t offset = 0;
		for (size_t curr; (curr = source.find(kPragma, offset)) != std::string_view::npos;) {
			result->append(source.substr(offset, curr - offset));

			offset = curr + kPragma.size();
			size_t consumed = procCommands(result, source.substr(offset));
			if (!result.has_value()) {
				return std::nullopt;
			}

			if (consumed == 0) {
				result->append(kPragma);
			}

			offset += consumed;
		}

		result->append(source.substr(offset));
		return result;
	}

	void ShaderPreprocessor::addIncludeDirs(const std::filesystem::path *dirs, size_t count) {
		if (dirs == nullptr)
			return;

		if (count == static_cast<size_t>(-1)) {
			for (count = 0; dirs + count != nullptr; ++count) {
			}
		}

		std::filesystem::path path;
		for (size_t n = 0; n < count; ++n) {
			path = dirs[n];
			path.make_preferred();

			m_includeDirs.push_back(std::move(path));
		}
	}

	size_t ShaderPreprocessor::procCommands(
	    std::optional<std::string> &result, std::string_view source
	) {
		size_t space = source.find(' ');
		if (space == std::string_view::npos)
			return 0;

		std::string_view command = source.substr(0, space);

		size_t consumed = 0;
		if (command == "include")
			consumed = procInclude(result, source.substr(space));
		else
			return 0;

		return consumed ? space + consumed : 0;
	}

	size_t ShaderPreprocessor::procInclude(
	    std::optional<std::string> &result, std::string_view source
	) {
		std::string_view line = source.substr(0, source.find('\0'));

		size_t begin = line.find('"');
		if (begin == std::string_view::npos) {
			log::Logger::moduleError(kModuleName, "Failed to find parenthesis.");
			result = std::nullopt;
			return 0;
		}

		size_t end = line.find('"', begin + 1);
		if (end == std::string_view::npos) {
			log::Logger::moduleError(kModuleName, "Failed to find parenthesis.");
			result = std::nullopt;
			return 0;
		}

		std::string dir = findDirectory(line.substr(begin + 1, end - begin - 1));
		if (dir.empty()) {
			log::Logger::moduleError(
			    kModuleName,
			    "Failed to find include directory for '{}'",
			    line.substr(begin + 1, end - begin - 1)
			);
			result = std::nullopt;
			return 0;
		}

		std::ifstream file(dir, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			log::Logger::moduleError(kModuleName, "Failed to open '{}'.", dir);
			result = std::nullopt;
			return 0;
		}

		std::streamsize fileSize = file.tellg();
		if (fileSize > 0) {
			file.seekg(0, std::ios::beg);

			size_t oldSize = result->size();
			result->resize(oldSize + fileSize);

			if (!file.read(result->data() + oldSize, fileSize)) {
				log::Logger::moduleError(kModuleName, "Access denied to file '{}'.", dir);
				result = std::nullopt;
				return 0;
			}
		}

		if (!result->empty() && result->back() != '\n')
			result->push_back('\n');

		return end + 1;
	}

	std::string ShaderPreprocessor::findDirectory(std::string_view name) {
		std::filesystem::path relative(name);
		relative.make_preferred();

		std::error_code ec;
		for (const auto &includeDir : m_includeDirs) {
			std::filesystem::path candidate = includeDir / relative;

			if (std::filesystem::is_regular_file(candidate, ec) && !ec) {
				auto absolute = std::filesystem::absolute(candidate, ec);
				if (!ec) {
					return absolute.string();
				}
			}
		}

		return {};
	}
} // namespace triple::game

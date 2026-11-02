#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <optional>

namespace triple::game {
	class ShaderPreprocessor {
		static constexpr std::string_view kModuleName = "ShaderPreprocessor";

	public:
		/*
		 * @brief Processes specified code for commands.
		 * @note Does not replaces unknown commands.
		 *
		 * @param source - Source code.
		 *
		 * @return Code cleaned out from known commands.
		 */
		std::optional<std::string> process(std::string_view source);

		/*
		 * @brief Adds specified 'include directories'.
		 *
		 * @param dirs - Include directories.
		 * @param count - Count of 'include directories' array.
		 *
		 * @note If 'count' is -1, then 'dirs' array must end with null-pointer
		 *       to detect the count.
		 */
		void addIncludeDirs(const std::filesystem::path *dirs, size_t count = -1);

	private:
		size_t procCommands(std::optional<std::string> &result, std::string_view source);
		size_t procInclude(std::optional<std::string> &result, std::string_view source);

		std::string findDirectory(std::string_view directory);

	private:
		std::vector<std::filesystem::path> m_includeDirs;
	};
} // namespace triple::game

#include "Path.hpp"

#include <format>

namespace Path
{
	bool createSettingsDirectory()
	{
		return std::filesystem::is_directory(Path::settingsDirectory) || std::filesystem::create_directory(std::filesystem::path{ Path::settingsDirectory });
	}

	std::filesystem::path imguiIni()
	{
		return std::format("{}\\imgui.ini", Path::settingsDirectory);
	}

	std::filesystem::path settingsFile(const char* file)
	{
		return std::format("{}\\{}", Path::settingsDirectory, file);
	}

	std::filesystem::path settingsProcessFile(const char* platform, const char* file)
	{
		return std::format("{}\\{}\\Process\\{}", Path::settingsDirectory, platform, file);
	}

	std::filesystem::path gamesSettings(const char* platform)
	{
		return std::format("{}\\{}\\Games", Path::settingsDirectory, platform);
	}
}
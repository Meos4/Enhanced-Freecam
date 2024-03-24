#pragma once

#include <filesystem>

namespace Path
{
	inline constexpr auto settingsDirectory{ "Settings" };

	bool createSettingsDirectory();
	std::filesystem::path imguiIni();
	std::filesystem::path settingsFile(const char* file);
	std::filesystem::path settingsProcessFile(const char* platform, const char* file);
	std::filesystem::path gamesSettings(const char* platform);
}
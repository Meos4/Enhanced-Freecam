#pragma once

#include "Common/GameInfo.hpp"
#include "Common/Process.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <atomic>
#include <filesystem>
#include <memory>
#include <span>

namespace PS2
{
	inline constexpr u32 memSize{ 0x02000000 };
	inline constexpr auto name{ "PlayStation 2" }, settingsName{ "PS2" };

	std::span<const GameInfo> games();
	std::span<const EmulatorInfo> emulators();
	std::filesystem::path settingsFilePath(const char* game);
	bool isValidMemoryRange(u32 offset);
	Ram createRam(std::shared_ptr<Process> process, std::uintptr_t begin);
	std::uintptr_t asyncGameSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);
}
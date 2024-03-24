#pragma once

#include "Common/GameInfo.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <atomic>
#include <filesystem>
#include <memory>
#include <span>

namespace PS1
{
	inline constexpr s32 fixedOne{ 1 << 12 };
	inline constexpr u32 memSize{ 0x00200000 };
	inline constexpr auto name{ "PlayStation 1" }, settingsName{ "PS1" };

	std::span<const GameInfo> games();
	std::span<const EmulatorInfo> emulators();
	std::filesystem::path settingsFilePath(const char* game);
	bool isValidMemoryRange(u32 offset);
	Ram createRam(std::shared_ptr<Process> process, std::uintptr_t begin);
	std::uintptr_t asyncGameSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);

	constexpr float fixedDecimal(s32 val)
	{
		return static_cast<float>(val / fixedOne);
	}

	constexpr float fixedFractional(s32 val)
	{
		return (static_cast<float>(val % fixedOne) / fixedOne);
	}

	constexpr float fixedToFloat(s32 val)
	{
		return PS1::fixedDecimal(val) + PS1::fixedFractional(val);
	}

	constexpr s32 floatToFixed(float val)
	{
		const auto decimal{ static_cast<s32>(val) };
		return (decimal << 12) + static_cast<s32>((val - decimal) * fixedOne);
	}
}
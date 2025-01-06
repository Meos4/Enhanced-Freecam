#pragma once

#include "Common/GameInfo.hpp"
#include "Common/Process.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"
#include "Common/Util.hpp"

#include <atomic>
#include <filesystem>
#include <optional>
#include <set>

#define PS2_PCSX2_DRAW_PNACH_BEHAVIOR_WINDOW PS2::PCSX2::drawPnachBehaviorWindow(&m_game)

namespace PS2::PCSX2
{
	struct PnachInfo
	{
		const char* crc;
		u32 textSectionBegin;
		u32 textSectionEnd;
	};

	bool createPnachForceJit
		(const std::filesystem::path& path, const Ram& ram, const std::set<u32>& offsets, const char* game, const char* version);
	std::set<u32> textSectionOffsets(u32* offset, std::size_t size, u32 textSectionBegin, u32 textSectionEnd);
	void drawCreatePnachTextSectionWindow(const Ram& ram, const std::set<u32>& offsets, const char* crc, const char* name, const char* version);
	std::filesystem::path defaultCheatsPath();
	std::optional<std::uintptr_t> eememPtr(const Process& process);
	std::optional<std::uintptr_t> eememVal(const Process& process);
	std::uintptr_t eememAGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);
	std::uintptr_t x86AGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);

	template <typename T>
	void drawPnachBehaviorWindow(T* game)
	{
		if (Util::isProcessName(game->ram().process(), "pcsx2"))
		{
			const auto& offsets{ game->offset() };
			const auto& pnachInfo{ game->pnachInfo() };
			const auto nbOffsets{ sizeof(offsets) / sizeof(u32) };
			const auto textSectionOffsets{ PCSX2::textSectionOffsets((u32*)&offsets, nbOffsets, pnachInfo.textSectionBegin, pnachInfo.textSectionEnd) };
			PCSX2::drawCreatePnachTextSectionWindow(game->ram(), textSectionOffsets, pnachInfo.crc, game->name, game->versionText(game->version()));
		}
	}
}
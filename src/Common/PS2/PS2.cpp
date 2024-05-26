#include "PS2.hpp"

#include "Games/PS2/DBZBudokai3/Game.hpp"
#include "Games/PS2/DBZTenkaichi3/Game.hpp"
#include "Games/PS2/DBZInfiniteWorld/Game.hpp"
#include "Games/PS2/DragonQuest8/Game.hpp"
#include "Games/PS2/HauntingGround/Game.hpp"
#include "Games/PS2/JadeCocoon2/Game.hpp"
#include "Games/PS2/ResidentEvil4/Game.hpp"
#include "Games/PS2/Sly1/Game.hpp"

#include "Common/AsyncGameSearcher.hpp"
#include "Common/Path.hpp"
#include "Common/RamRW.hpp"
#include "Common/Util.hpp"

#include "PCSX2.hpp"

#include <array>
#include <filesystem>
#include <format>

namespace PS2
{
	std::span<const GameInfo> games()
	{
		static constexpr std::array<GameInfo, 8> games
		{
			ADD_GAME_INFO(DBZBudokai3),
			ADD_GAME_INFO(DBZTenkaichi3),
			ADD_GAME_INFO(DBZInfiniteWorld),
			ADD_GAME_INFO(DragonQuest8),
			ADD_GAME_INFO(HauntingGround),
			ADD_GAME_INFO(JadeCocoon2),
			ADD_GAME_INFO(ResidentEvil4),
			ADD_GAME_INFO(Sly1)
		};

		return games;
	}

	std::span<const EmulatorInfo> emulators()
	{
		static constexpr std::array<EmulatorInfo, 1> emu
		{{
			{ "PCSX2", "pcsx2" }
		}};

		return emu;
	}

	std::filesystem::path settingsFilePath(const char* game)
	{
		const auto path{ Path::gamesSettings(PS2::settingsName) };
		return std::format("{}/{}.json", path.string().c_str(), game);
	}

	bool isValidMemoryRange(u32 offset)
	{
		return offset && !(offset > PS2::memSize);
	}

	Ram createRam(std::shared_ptr<Process> process, std::uintptr_t begin)
	{
		bool noProtect{};

		if (PCSX2::eememPtr(*process).has_value())
		{
			noProtect = true;
		}

		return Ram{ process, std::make_unique<RamRWEmulator>(process.get(), begin, noProtect), PS2::memSize };
	}

	std::uintptr_t asyncGameSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
	{
		if (Util::isProcessName(process, "pcsx2") && process.architecture() == Process::Architecture::x86)
		{
			return PCSX2::x86AGS(process, running, op);
		}
		if (PCSX2::eememPtr(process).has_value())
		{
			return PCSX2::eememAGS(process, running, op);
		}

		return AsyncGameSearcher::emulatorSearcher(process, running, op, PS2::memSize);
	}
}
#include "PS1.hpp"

#include "Games/PS1/ApeEscape/Game.hpp"
#include "Games/PS1/DinoCrisis/Game.hpp"

#include "Common/AsyncGameSearcher.hpp"
#include "Common/Path.hpp"
#include "Common/Util.hpp"

#include "DuckStation.hpp"

#include <array>
#include <format>

namespace PS1
{
	std::span<const GameInfo> games()
	{
		static constexpr std::array<GameInfo, 2> games
		{
			ADD_GAME_INFO(ApeEscape),
			ADD_GAME_INFO(DinoCrisis)
		};

		return games;
	}

	std::span<const EmulatorInfo> emulators()
	{
		static constexpr std::array<EmulatorInfo, 2> emu
		{{
			{ "DuckStation", "duckstation" },
			{ "NO$PSX", "no$psx" }
		}};

		return emu;
	}

	std::filesystem::path settingsFilePath(const char* game)
	{
		const auto path{ Path::gamesSettings(PS1::settingsName) };
		return std::format("{}/{}.json", path.string().c_str(), game);
	}

	bool isValidMemoryRange(u32 offset)
	{
		return offset && !(offset > PS1::memSize);
	}

	Ram createRam(std::shared_ptr<Process> process, std::uintptr_t begin)
	{
		bool noProtect{};

		if (Util::isProcessName(*process, "duckstation"))
		{
			noProtect = true;
		}

		return Ram{ process, begin, PS1::memSize, noProtect };
	}

	std::uintptr_t asyncGameSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
	{
		if (DuckStation::ramPtr(process).has_value())
		{
			return DuckStation::ramAGS(process, running, op);
		}

		return AsyncGameSearcher::emulatorSearcher(process, running, op, PS1::memSize);
	}
}
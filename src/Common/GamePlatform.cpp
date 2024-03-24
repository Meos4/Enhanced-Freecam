#include "GamePlatform.hpp"

#include "PS1/PS1.hpp"
#include "PS2/PS2.hpp"

#include <array>

namespace GamePlatform
{
	const char* name(s32 platform)
	{
		static constexpr std::array<const char*, GamePlatform::Count> names
		{
			PS1::name,
			PS2::name
		};

		return names[platform];
	}

	std::span<const GameInfo> gamesInfo(s32 platform)
	{
		static constexpr std::array<std::span<const GameInfo>(*)(), GamePlatform::Count> gameInfos
		{
			PS1::games,
			PS2::games
		};

		return gameInfos[platform]();
	}
}
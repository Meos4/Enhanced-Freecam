#pragma once

#include "GameInfo.hpp"
#include "Types.hpp"

#include <span>

namespace GamePlatform
{
	enum
	{
		PS1,
		PS2,
		Count
	};

	const char* name(s32 platform);
	std::span<const GameInfo> gamesInfo(s32 platform);
}
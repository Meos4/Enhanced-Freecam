#pragma once

#include "Common/Types.hpp"

namespace PS2::DBZTenkaichi3
{
	class Game;

	namespace CustomCode
	{
		inline constexpr auto viewMatrixSize{ sizeof(float[4][4]) };

		void update(const Game& game);
		bool isApplied(const Game& game);
		u32 viewMatrixOffset(const Game& game);
	}
}
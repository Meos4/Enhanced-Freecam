#pragma once

#include "Common/Types.hpp"

namespace PS2::JadeCocoon2
{
	class Game;

	namespace CustomCode
	{
		inline constexpr auto viewMatrixSize{ sizeof(float[4][4]) - sizeof(float) };

		void update(const Game& game);
		bool isApplied(const Game& game);
		u32 viewMatrixOffset(const Game& game);
	}
}
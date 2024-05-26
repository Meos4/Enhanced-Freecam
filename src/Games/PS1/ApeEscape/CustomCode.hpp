#pragma once

#include "Common/Types.hpp"

namespace PS1::ApeEscape
{
	class Game;

	namespace CustomCode
	{
		void update(const Game& game);
		bool isApplied(const Game& game);
		u32 viewMatrixOffset(const Game& game);
	}
}
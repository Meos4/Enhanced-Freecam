#pragma once

#include "Common/PS2/libvu0.hpp"

#include "Common/Types.hpp"

namespace PS2::JadeCocoon2
{
	class Game;

	namespace CustomCode
	{
		inline constexpr auto viewMatrixSize{ sizeof(libvu0::sceVu0FMATRIX) - sizeof(float) };

		void update(const Game& game);
		bool isApplied(const Game& game);
		u32 viewMatrixOffset(const Game& game);
	}
}
#pragma once

#include "Common/Types.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	class Game;

	namespace CustomCode
	{
		using Packet = std::array<float, 52>;

		void update(const Game& game);
		bool isApplied(const Game& game);
		u32 fnSetPacketOffset(const Game& game);
		u32 packetOffset(const Game& game);
		u32 fnSetFovOffset(const Game& game);
		u32 fovOffset(const Game& game);
	}
}
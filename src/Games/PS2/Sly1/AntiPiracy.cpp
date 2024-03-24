#include "AntiPiracy.hpp"

#include "Game.hpp"

namespace PS2::Sly1::AntiPiracy
{
	void disable(const Game& game)
	{
		const auto& ram{ game.ram() };
		const auto& offset{ game.offset() };

		ram.write(offset.Fn_antiPiracy1 + 0xB4, 0x00001021);
		ram.write(offset.Fn_antiPiracy2 + 0xC8, 0x00001021);
		ram.write(offset.Fn_antiPiracy3 + 0x134, 0x00001021);
		ram.write(offset.Fn_antiPiracy4 + 0xD8, 0x10000008);
	}
}
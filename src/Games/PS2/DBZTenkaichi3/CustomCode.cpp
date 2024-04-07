#include "CustomCode.hpp"

#include "Common/Mips.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DBZTenkaichi3::CustomCode
{
	using Fn_SetViewMatrix = std::array<Mips_t, 9>;

	void update(const Game& game)
	{
		const auto& offset{ game.offset() };
		const auto li32_viewMatrixOffset{ Mips::li32(Mips::Register::a1, CustomCode::viewMatrixOffset(game)) };

		const CustomCode::Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x7FBF0000, // sq ra,(sp)
			li32_viewMatrixOffset[0],
			li32_viewMatrixOffset[1],
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, CustomCode::viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		game.ram().write(game.offset().Fn_unknown, Fn_setViewMatrix);
	}

	bool isApplied(const Game& game)
	{
		return game.ram().read<Mips_t>(game.offset().Fn_unknown) == 0x27BDFFF0;
	}

	u32 viewMatrixOffset(const Game& game)
	{
		return game.offset().Fn_unknown + sizeof(CustomCode::Fn_SetViewMatrix);
	}
}
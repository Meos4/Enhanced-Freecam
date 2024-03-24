#include "CustomCode.hpp"

#include "Common/Mips.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::JadeCocoon2::CustomCode
{
	using Fn_SetViewMatrix = std::array<Mips_t, 7>;

	void update(const Game& game)
	{
		const auto& offset{ game.offset() };

		const CustomCode::Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x7FBF0000, // sq ra,(sp)
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, CustomCode::viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		game.ram().write(offset.Fn_std___BCD___BCD, Fn_setViewMatrix);
	}

	bool isApplied(const Game& game)
	{
		return game.ram().read<Mips_t>(game.offset().Fn_std___BCD___BCD) == 0x27BDFFF0;
	}

	u32 viewMatrixOffset(const Game& game)
	{
		return game.offset().Fn_std___BCD___BCD + sizeof(CustomCode::Fn_SetViewMatrix);
	}
}
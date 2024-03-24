#include "CustomCode.hpp"

#include "Game.hpp"

namespace PS1::ApeEscape::CustomCode
{
	using Fn_SetViewMatrix = std::array<Mips_t, 10>;

	void update(const Game& game)
	{
		const auto& offset{ game.offset() };
		const auto li32_viewMatrixOffset{ Mips::li32(Mips::Register::a1, 0x80000000 + CustomCode::viewMatrixOffset(game)) };

		const CustomCode::Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0xAFBF0000, // sw ra,(sp)
			li32_viewMatrixOffset[0],
			li32_viewMatrixOffset[1],
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 32),
			0x8FBF0000, // lw ra,(sp)
			0x00000000, // nop
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		game.ram().write(offset.sonyLibrary, Fn_setViewMatrix);
	}

	bool isApplied(const Game& game)
	{
		return game.ram().read<u32>(game.offset().sonyLibrary) != 0x7262694C;
	}

	u32 viewMatrixOffset(const Game& game)
	{
		return game.offset().sonyLibrary + sizeof(CustomCode::Fn_SetViewMatrix);
	}
}
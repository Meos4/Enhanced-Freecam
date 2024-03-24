#include "CustomCode.hpp"

#include "Common/Mips.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::HauntingGround::CustomCode
{
	using Fn_SetViewMatrixIfMainCamera = std::array<Mips_t, 15>;

	void update(const Game& game)
	{
		const auto& offset{ game.offset() };

		const auto
			li32_mainMatrixCallSetMatrixReturnOffset{ Mips::li32(Mips::Register::v1, offset.Fn_setMainMatrix + 0x28) },
			li32_viewMatrixOffset{ Mips::li32(Mips::Register::a1, CustomCode::viewMatrixOffset(game)) };

		const CustomCode::Fn_SetViewMatrixIfMainCamera Fn_svmifmc
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x8FA200F0, // lw v0, 0xF0(sp)
			li32_mainMatrixCallSetMatrixReturnOffset[0],
			li32_mainMatrixCallSetMatrixReturnOffset[1],
			0x10430003, // beq v0, v1, +3
			0x7FBF0000, // sq ra,(sp)
			Mips::j(offset.Fn_sceVu0CameraMatrix),
			0x27BD0010, // addiu sp, 0x10
			li32_viewMatrixOffset[0],
			li32_viewMatrixOffset[1],
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, CustomCode::viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		game.ram().write(offset.Fn_std__default_new_handler, Fn_svmifmc);
	}

	bool isApplied(const Game& game)
	{
		return game.ram().read<Mips_t>(game.offset().Fn_std__default_new_handler) == 0x27BDFFF0;
	}

	u32 viewMatrixOffset(const Game& game)
	{
		return game.offset().Fn_std__default_new_handler + sizeof(CustomCode::Fn_SetViewMatrixIfMainCamera);
	}
}
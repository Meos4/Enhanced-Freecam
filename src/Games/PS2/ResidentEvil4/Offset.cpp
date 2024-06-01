#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	static constexpr Offset
	offsetPal
	{
		.Fn_memcpy = 0x002AF684,
		.cameraPtr = 0x00325588,
		.menuStruct = 0x00417800,
		.playerPtr = 0x00420650,
		.progression = 0x004311E4,
		.Fn_drawFog = 0x002160C8,
		.Fn_drawHud = 0x0021E420,
		.Fn_drawText = 0x0012C320,
		.Fn_padStatus = 0x0023E7C8,
		.Fn_setMatrix = 0x001AD500,
		.Fn_transposeMatrix = 0x00252F00,
		.Fn_unknown = 0x00252FE8,
		.Fn_unknown2 = 0x00203A78,
		.Fn_updateAction = 0x00274478,
		.Fn_updateExtern = 0x002335B0,
		.Fn_updateGameOver = 0x0021A040,
		.Fn_updateItems = 0x00223768,
		.Fn_updateNpc = 0x001BB078,
		.Fn_updatePlayer = 0x00166DD0,
		.Fn_updatePlayerCollisions = 0x001A19D0,
		.Fn_updateSfx = 0x001F6318,
		.Fn_updateSfx2 = 0x001EA1A0,
		.Fn_updateTimer = 0x001ADCA0
	},
	offsetNtscU
	{
		.Fn_memcpy = 0x002AF3AC,
		.cameraPtr = 0x00325308,
		.menuStruct = 0x00417580,
		.playerPtr = 0x004203D0,
		.progression = 0x00430F64,
		.Fn_drawFog = 0x00215FE0,
		.Fn_drawHud = 0x0021E338,
		.Fn_drawText = 0x0012C208,
		.Fn_padStatus = 0x0023E650,
		.Fn_setMatrix = 0x001AD418,
		.Fn_transposeMatrix = 0x00252D88,
		.Fn_unknown = 0x00252E70,
		.Fn_unknown2 = 0x00203990,
		.Fn_updateAction = 0x00274300,
		.Fn_updateExtern = 0x00233438,
		.Fn_updateGameOver = 0x00219F58,
		.Fn_updateItems = 0x00223680,
		.Fn_updateNpc = 0x001BAF90,
		.Fn_updatePlayer = 0x00166CE8,
		.Fn_updatePlayerCollisions = 0x001A18E8,
		.Fn_updateSfx = 0x001F6230,
		.Fn_updateSfx2 = 0x001EA0B8,
		.Fn_updateTimer = 0x001ADBB8
	},
	offsetNtscJ
	{
		.Fn_memcpy = 0x002AFFAC,
		.cameraPtr = 0x00325F38,
		.menuStruct = 0x00418240,
		.playerPtr = 0x00421090,
		.progression = 0x00431C24,
		.Fn_drawFog = 0x002169E0,
		.Fn_drawHud = 0x0021EDB0,
		.Fn_drawText = 0x0012C420,
		.Fn_padStatus = 0x0023F0F8,
		.Fn_setMatrix = 0x001ADC08,
		.Fn_transposeMatrix = 0x00253828,
		.Fn_unknown = 0x00253910,
		.Fn_unknown2 = 0x00204390,
		.Fn_updateAction = 0x00274EB0,
		.Fn_updateExtern = 0x00233EA8,
		.Fn_updateGameOver = 0x0021A958,
		.Fn_updateItems = 0x002240F8,
		.Fn_updateNpc = 0x001BB778,
		.Fn_updatePlayer = 0x001674D8,
		.Fn_updatePlayerCollisions = 0x001A20D8,
		.Fn_updateSfx = 0x001F6C30,
		.Fn_updateSfx2 = 0x001EAA88,
		.Fn_updateTimer = 0x001AE3A8
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetPal,
			&offsetNtscU,
			&offsetNtscJ
		};

		return *offsets[version];
	}
}
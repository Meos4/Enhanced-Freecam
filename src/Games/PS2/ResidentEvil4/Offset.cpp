#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	static constexpr Offset
	offsetNtscJ
	{
		.Fn_memcpy = 0x002AFFAC,
		.cameraPtr = 0x00325F38,
		.menuStruct = 0x00418240,
		.progression = 0x00431C24,
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
		.Fn_updateSfx = 0x001F6C30,
		.Fn_updateSfx2 = 0x001EAA88,
		.Fn_updateTimer = 0x001AE3A8
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetNtscJ
		};

		return *offsets[version];
	}
}
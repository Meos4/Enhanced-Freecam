#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DBZTenkaichi3
{
	static constexpr Offset
	offsetPal
	{
		.Fn_memcpy = 0x002AA9DC,
		.Fn_sceVu0InversMatrix = 0x00120340,
		.battlePlayerPtr = 0x002FFF94,
		.flags = 0x003305F8,
		.Fn_battleDrawHud = 0x00212DD0,
		.Fn_battleSetPlayerCamera = 0x0023F170,
		.Fn_cutsceneUpdateCamera = 0x0023DBD0,
		.Fn_drawFarBlur = 0x002460C8,
		.Fn_drawNearBlur = 0x00103068,
		.Fn_padStatus = 0x00122AD8,
		.Fn_setViewMatrix = 0x0023ECB0,
		.Fn_unknown = 0x002A9820,
		.Fn_viewModelUpdateCamera = 0x0023FDA8
	};
	
	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetPal
		};

		return *offsets[version];
	}
}
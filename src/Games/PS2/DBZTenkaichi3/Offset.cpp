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
		.progressionPtr = 0x0030070C,
		.splitscreen = 0x0032EC2C,
		.viewModelPtr = 0x003B2154,
		.Fn_battleDrawHud = 0x00212DD0,
		.Fn_battleSetPlayerCamera = 0x0023F170,
		.Fn_cutsceneUpdateCamera = 0x0023DBD0,
		.Fn_drawAuras = 0x00164D98,
		.Fn_drawAurasLightning = 0x001684B8,
		.Fn_drawBlurCutscene = 0x002487D8,
		.Fn_drawCharTextures = 0x00111838,
		.Fn_drawFarBlur = 0x002460C8,
		.Fn_drawNearBlur = 0x00103068,
		.Fn_drawShade = 0x00196AA8,
		.Fn_drawSprite = 0x0010D800,
		.Fn_drawWhiteLines = 0x0017F620,
		.Fn_drawWhiteScreen = 0x00172C98,
		.Fn_padStatus = 0x00122AD8,
		.Fn_setViewMatrix = 0x0023ECB0,
		.Fn_unknown = 0x002A9820,
		.Fn_updateChar = 0x0024B1E8,
		.Fn_updateCharBlinkEyes = 0x0024FAA0,
		.Fn_updateCharVisibility = 0x0024B380,
		.Fn_updateCharVisibility2 = 0x0024B470,
		.Fn_updateCharNear = 0x0024AE28,
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
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
		.replayMode = 0x00318C34,
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
		.Fn_updateShenron = 0x002632E0,
		.Fn_viewModelUpdateCamera = 0x0023FDA8
	},
	offsetNtscU
	{
		.Fn_memcpy = 0x002A9A1C,
		.Fn_sceVu0InversMatrix = 0x001202A0,
		.battlePlayerPtr = 0x002FEB14,
		.flags = 0x003337B8,
		.progressionPtr = 0x002FF28C,
		.replayMode = 0x0031BE04,
		.splitscreen = 0x00331DEC,
		.viewModelPtr = 0x003B4854,
		.Fn_battleDrawHud = 0x002129C8,
		.Fn_battleSetPlayerCamera = 0x0023EAD0,
		.Fn_cutsceneUpdateCamera = 0x0023D510,
		.Fn_drawAuras = 0x00164B40,
		.Fn_drawAurasLightning = 0x00168258,
		.Fn_drawBlurCutscene = 0x00248160,
		.Fn_drawCharTextures = 0x00111788,
		.Fn_drawFarBlur = 0x00245A50,
		.Fn_drawNearBlur = 0x00103070,
		.Fn_drawShade = 0x001967E0,
		.Fn_drawSprite = 0x0010D750,
		.Fn_drawWhiteLines = 0x0017F390,
		.Fn_drawWhiteScreen = 0x001729F0,
		.Fn_padStatus = 0x00122A38,
		.Fn_setViewMatrix = 0x0023E608,
		.Fn_unknown = 0x002A8860,
		.Fn_updateChar = 0x0024AB70,
		.Fn_updateCharBlinkEyes = 0x0024F428,
		.Fn_updateCharVisibility = 0x0024AD08,
		.Fn_updateCharVisibility2 = 0x0024ADF8,
		.Fn_updateCharNear = 0x0024A7B0,
		.Fn_updateShenron = 0x00262C30,
		.Fn_viewModelUpdateCamera = 0x0023F708
	},
	offsetNtscJ
	{
		.Fn_memcpy = 0x002A97CC,
		.Fn_sceVu0InversMatrix = 0x001202A8,
		.battlePlayerPtr = 0x002FEB14,
		.flags = 0x003337B8,
		.progressionPtr = 0x002FF28C,
		.replayMode = 0x0031BE04,
		.splitscreen = 0x00331DEC,
		.viewModelPtr = 0x003B4454,
		.Fn_battleDrawHud = 0x00212888,
		.Fn_battleSetPlayerCamera = 0x0023E920,
		.Fn_cutsceneUpdateCamera = 0x0023D360,
		.Fn_drawAuras = 0x00164AE0,
		.Fn_drawAurasLightning = 0x001681F8,
		.Fn_drawBlurCutscene = 0x00247FB0,
		.Fn_drawCharTextures = 0x00111788,
		.Fn_drawFarBlur = 0x002458A0,
		.Fn_drawNearBlur = 0x00103070,
		.Fn_drawShade = 0x00196770,
		.Fn_drawSprite = 0x0010D750,
		.Fn_drawWhiteLines = 0x0017F320,
		.Fn_drawWhiteScreen = 0x00172980,
		.Fn_padStatus = 0x00122A40,
		.Fn_setViewMatrix = 0x0023E458,
		.Fn_unknown = 0x002A8610,
		.Fn_updateChar = 0x0024A9C0,
		.Fn_updateCharBlinkEyes = 0x0024F278,
		.Fn_updateCharVisibility = 0x0024AB58,
		.Fn_updateCharVisibility2 = 0x0024AC48,
		.Fn_updateCharNear = 0x0024A600,
		.Fn_updateShenron = 0x00262AA0,
		.Fn_viewModelUpdateCamera = 0x0023F558
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
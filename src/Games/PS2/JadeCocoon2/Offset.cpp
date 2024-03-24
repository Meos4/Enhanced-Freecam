#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::JadeCocoon2
{
	static constexpr Offset
	offsetPal
	{
		.gGMEP_SceneEventTable = 0x006C4658,
		.gM2MainEnv = 0x006D69D0,
		.newPause = 0x006C50AC,
		.Fn_EvM2Camera_Control = 0x001A5440,
		.Fn__gnkPadman_StandardStatus = 0x00157AD0,
		.Fn_M2AutoMap_Draw = 0x00246490,
		.Fn_m2Camera_ExeFcurve = 0x001A7B40,
		.Fn_m2Camera_Init = 0x001A5E60,
		.Fn_m2Camera_InitFixedPos = 0x001A7430,
		.Fn_m2Fix_SjisDispString = 0x00164DD0,
		.Fn_m2Fix_SjisDispStringEx = 0x00164E50,
		.Fn_m2Fix_SjisDispValue = 0x00164E20,
		.Fn_m2MapCamera = 0x001A6440,
		.Fn_m2Sprite_Disp = 0x00161C00,
		.Fn_m2Window_DispWindow = 0x001671C0,
		.Fn_memcpy = 0x00132AB8,
		.Fn_sceVu0CameraMatrix = 0x00111148,
		.Fn_std___BCD___BCD = 0x001071D0,
		.Fn__XVIVIEW__SetMatrix = 0x0013E920
	},
	offsetNtscU
	{
		.gGMEP_SceneEventTable = 0x006C3E58,
		.gM2MainEnv = 0x006D61D0,
		.newPause = 0x006C48AC,
		.Fn_EvM2Camera_Control = 0x001A53A0,
		.Fn__gnkPadman_StandardStatus = 0x00157AD0,
		.Fn_M2AutoMap_Draw = 0x002461F0,
		.Fn_m2Camera_ExeFcurve = 0x001A7AA0,
		.Fn_m2Camera_Init = 0x001A5DC0,
		.Fn_m2Camera_InitFixedPos = 0x001A7390,
		.Fn_m2Fix_SjisDispString = 0x00164DE0,
		.Fn_m2Fix_SjisDispStringEx = 0x00164E60,
		.Fn_m2Fix_SjisDispValue = 0x00164E30,
		.Fn_m2MapCamera = 0x001A63A0,
		.Fn_m2Sprite_Disp = 0x00161C10,
		.Fn_m2Window_DispWindow = 0x001671D0,
		.Fn_memcpy = 0x00132AB8,
		.Fn_sceVu0CameraMatrix = 0x00111148,
		.Fn_std___BCD___BCD = 0x001071D0,
		.Fn__XVIVIEW__SetMatrix = 0x0013E920
	},
	offsetNtscJ
	{
		.gGMEP_SceneEventTable = 0x006C17C8,
		.gM2MainEnv = 0x006D3B50,
		.newPause = 0x006C222C,
		.Fn_EvM2Camera_Control = 0x001A5700,
		.Fn__gnkPadman_StandardStatus = 0x00157B60,
		.Fn_M2AutoMap_Draw = 0x00243660,
		.Fn_m2Camera_ExeFcurve = 0x001A7E00,
		.Fn_m2Camera_Init = 0x001A6120,
		.Fn_m2Camera_InitFixedPos = 0x001A76F0,
		.Fn_m2Fix_SjisDispString = 0x00165160,
		.Fn_m2Fix_SjisDispStringEx = 0x001651E0,
		.Fn_m2Fix_SjisDispValue = 0x001651B0,
		.Fn_m2MapCamera = 0x001A6700,
		.Fn_m2Sprite_Disp = 0x00161D50,
		.Fn_m2Window_DispWindow = 0x001677D0,
		.Fn_memcpy = 0x00132B70,
		.Fn_sceVu0CameraMatrix = 0x00111148,
		.Fn_std___BCD___BCD = 0x001071D0,
		.Fn__XVIVIEW__SetMatrix = 0x0013E9D0
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
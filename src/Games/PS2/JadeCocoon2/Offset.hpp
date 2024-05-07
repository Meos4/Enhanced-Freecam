#pragma once

#include "Common/Types.hpp"

namespace PS2::JadeCocoon2
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		// Pal .XMA
		u32 gGMEP_SceneEventTable,
			gM2MainEnv,
			newPause,
			Fn_EvM2Camera_Control,
			Fn__gnkPadman_StandardStatus,
			Fn_M2AutoMap_Draw,
			Fn_m2Camera_ExeFcurve,
			Fn_m2Camera_Init,
			Fn_m2Camera_InitFixedPos,
			Fn_m2Fix_SjisDispString,
			Fn_m2Fix_SjisDispStringEx,
			Fn_m2Fix_SjisDispValue,
			Fn_m2MapCamera,
			Fn_m2Selector_DispIcon,
			Fn_m2Sprite_Disp,
			Fn_m2Window_DispWindow,
			Fn_memcpy,
			Fn_sceVu0CameraMatrix,
			Fn_std___BCD___BCD,
			Fn__XVIVIEW__SetMatrix;
	};
}
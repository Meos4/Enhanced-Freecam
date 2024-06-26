#pragma once

#include "Common/Types.hpp"

namespace PS2::DBZTenkaichi3
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 Fn_memcpy,
			Fn_sceVu0InversMatrix,
			battlePlayerPtr,
			flags,
			progressionPtr,
			replayMode,
			splitscreen,
			viewModelPtr,
			Fn_battleDrawHud,
			Fn_battleSetPlayerCamera,
			Fn_cutsceneUpdateCamera,
			Fn_drawAuras,
			Fn_drawAurasLightning,
			Fn_drawBlurCutscene,
			Fn_drawCharTextures,
			Fn_drawFarBlur,
			Fn_drawNearBlur,
			Fn_drawShade,
			Fn_drawSprite,
			Fn_drawWhiteLines,
			Fn_drawWhiteScreen,
			Fn_padStatus,
			Fn_setViewMatrix,
			Fn_unknown,
			Fn_updateChar,
			Fn_updateCharBlinkEyes,
			Fn_updateCharVisibility,
			Fn_updateCharVisibility2,
			Fn_updateCharNear,
			Fn_updateShenron,
			Fn_viewModelUpdateCamera;
	};
}
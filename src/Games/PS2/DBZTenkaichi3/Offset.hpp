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
			Fn_battleDrawHud,
			Fn_battleSetPlayerCamera,
			Fn_cutsceneUpdateCamera,
			Fn_drawAuras,
			Fn_drawCharTextures,
			Fn_drawFarBlur,
			Fn_drawNearBlur,
			Fn_padStatus,
			Fn_setViewMatrix,
			Fn_unknown,
			Fn_viewModelUpdateCamera;
	};
}
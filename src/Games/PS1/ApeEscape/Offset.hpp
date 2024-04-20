#pragma once

#include "Common/Types.hpp"

namespace PS1::ApeEscape
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 Fn_memcpy,
			billboardMatrix,
			cameraPosition,
			cutsceneState,
			fog,
			globalStruct,
			minigame,
			overlay,
			projectionMatrix,
			sonyLibrary,
			viewMatrix,
			Fn_drawActive,
			Fn_drawEnemy,
			Fn_drawLayer,
			Fn_drawLayer2,
			Fn_drawText,
			Fn_inGameLoop,
			Fn_padStatus,
			Fn_setViewMatrix,
			Fn_updateAdditional,
			Fn_updateCollectable,
			Fn_updateMonkey,
			Fn_updateMovable;
	};
}
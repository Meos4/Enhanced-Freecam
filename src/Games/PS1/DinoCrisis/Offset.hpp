#pragma once

#include "Common/Types.hpp"

namespace PS1::DinoCrisis
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 Fn_RotMatrix,
			dinosaurCamera,
			mainCamera,
			drawFadeTbl,
			Fn_dinosaurEntranceCameraTransition,
			Fn_drawEffects,
			Fn_drawFade,
			Fn_moveNextCamera,
			Fn_padStatus,
			Fn_setCameraPosition,
			Fn_setNextFrameCamera,
			Fn_updateCamera,
			Fn_updateCharacters,
			Fn_updateGameOver,
			Fn_updateInteractable,
			Fn_updateItems,
			Fn_updateLinearCutsceneCamera,
			Fn_updateMainCamera,
			Fn_updateMap,
			Fn_updateSfx;
	};
}
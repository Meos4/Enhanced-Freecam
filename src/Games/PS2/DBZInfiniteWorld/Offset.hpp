#pragma once

#include "Common/Types.hpp"

namespace PS2::DBZInfiniteWorld
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		// MSNF = Free Movement
		// MSN006 = Gohan's Training
		// MSN007 = 1,2,3,4! - (Goku's Training)
		// MSN010 = Fire Chucle Beams! - (Frieza FPS)
		// MSN012 = Tough Training! - (Vegeta's Training)
		// MSN013 = Hyperbolic Time Chamber (Trunks)
		// MSN014 = Hyperbolic Time Chamber (Gohan)
		// MSN019 = Flight training! - (Videl's Training)
		// SHOOT = Destroy the bricks!
		// FLIGHT = Dragon Universe

		u32 battleCameraPtr,
			battleCommonCamera,
			battleIsPaused,
			battleIsPauseMenuEnabled,
			battlePadData,
			battleTimescale,
			FLIGHT_cameraPtr,
			padData,
			progression,
			Fn_battleDrawDuelingInfo,
			Fn_battleSetCameraPosition,
			Fn_battleSetQuaternion,
			Fn_battleShouldDrawHud,
			Fn_battleUpdateCamera,
			Fn_battleUpdatePauseMenu,
			Fn_drawAspectRatio,
			Fn_drawAura,
			Fn_drawGreenRectangle,
			Fn_drawGreyLines,
			Fn_drawModel,
			Fn_drawSprites,
			Fn_drawText,
			Fn_drawTexturesOnShaders,
			Fn_drawWhiteScreen,
			Fn_FLIGHT_draw,
			Fn_FLIGHT_nextStep,
			Fn_FLIGHT_PLAYER,
			Fn_FLIGHT_UPDATE,
			Fn_FLIGHT_updateCamera,
			Fn_FLIGHT_cameraTransition,
			Fn_initMovies,
			Fn_initSelectStages,
			Fn_initTitleScreen,
			Fn_mdDrawHud,
			Fn_mdKrilin_PLY_POST_EXEC,
			Fn_mdKrilin_PLY_PRE_EXEC,
			Fn_mdTenshinhan_PLY_POST_EXEC,
			Fn_mdTenshinhan_PLY_PRE_EXEC,
			Fn_mdCutsceneNextStep,
			Fn_MSN006_NPC_PRE,
			Fn_MSN006_PLY_PRE,
			Fn_MSN006_Upd,
			Fn_MSN007_Mai,
			Fn_MSN007_POST_EXE,
			Fn_MSN007_PRE_EXE,
			Fn_MSN010_PLY_POST_EXE,
			Fn_MSN010_PLY_PRE_EXE,
			Fn_MSN012_Mai,
			Fn_MSN012_POST_EXE,
			Fn_MSN012_PRE_EXE,
			Fn_MSN012_Upd,
			Fn_MSN013_NPC_PRE,
			Fn_MSN013_PLY_PRE,
			Fn_MSN013_Upd,
			Fn_MSN013_updateLongBars,
			Fn_MSN014_NPC_PRE,
			Fn_MSN014_PLY_PRE,
			Fn_MSN014_updateBars,
			Fn_MSN019_PLY_POST_EXEC,
			Fn_MSN019_PLY_PRE_EXEC,
			Fn_MSN019_UPDATE,
			Fn_MSNF_FIX_DRAW,
			Fn_MSNF_PLY_POST_EXEC,
			Fn_MSNF_PLY_PRE_EXEC,
			Fn_MSNF_updateCamera,
			Fn_padStatus,
			Fn_setCutsceneCameraPosition,
			Fn_setProjectionMatrix,
			Fn_setProjectionMatrixFov,
			Fn_setTextures,
			Fn_SHOOT_update,
			Fn_unknown,
			Fn_unknown2;
	};
}
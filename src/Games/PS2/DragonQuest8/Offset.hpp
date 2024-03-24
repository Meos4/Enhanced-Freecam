#pragma once

#include "Common/Types.hpp"

namespace PS2::DragonQuest8
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		// Premium Disc
		u32 Fn_CreatePose__7CCameraFv,
			Fn_DrawStep__12CModelObjectFf,
			Fn_mgAddVector__FPfPf,
			Fn_Roll__9mgCCameraFf,
			Fn_SetPos__7CCameraFPf,
			Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo,
			Fn_Step__14CCameraControlFf,
			Fn_TimeStep__8dqCSceneFf,
		//
			alchemyPtr,
			bag,
			cameraPtr,
			cutsceneState,
			fov,
			gameState,
			heroStats,
			projectionStruct,
			timer,
			walkSpeed,
			Fn_drawBattleHud,
			Fn_drawCommonHud,
			Fn_drawCompass,
			Fn_drawMainCharacter,
			Fn_drawMidTextOnScreen,
			Fn_drawSkybox,
			Fn_drawTextOnScreen,
			Fn_npcScript,
			Fn_npcStep,
			Fn_nullsub1,
			Fn_padStatus,
			Fn_playerStep,
			Fn_randomEncounterBehavior,
			Fn_updateBattle;
	};
}
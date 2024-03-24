#pragma once

#include "Common/Types.hpp"

namespace PS2::DBZBudokai3
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 battleCameraPtr,
			battleCommonCamera,
			battleIsPaused,
			battleIsPauseMenuEnabled,
			battleTimescale,
			capsules,
			displayMode,
			displayMode2,
			duBackgroundPtr,
			duPosition,
			globalFov,
			progression,
			Fn_battleDrawDuelingInfo,
			Fn_battleSetCameraPosition,
			Fn_battleSetQuaternion,
			Fn_battleShouldDrawHud,
			Fn_battleUpdateCamera,
			Fn_battleUpdatePauseMenu,
			Fn_duACTDRAW,
			Fn_duBALL_DRAW,
			Fn_duDrawDragonUniverse,
			Fn_duDrawMap,
			Fn_duMAP,
			Fn_duPOS,
			Fn_duUpdateCamera,
			Fn_drawAura,
			Fn_drawGreyLines,
			Fn_drawModel,
			Fn_drawWhiteScreen,
			Fn_padStatus,
			Fn_setTextures,
			Fn_unknown;
	};
}
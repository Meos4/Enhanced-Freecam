#pragma once

#include "Common/Types.hpp"

namespace PS2::ResidentEvil4
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		// .sndata
		u32 pG,
			pPL,
			SubScreenWk,
			SystemSave,
			Fn_C_MTXLookAt,
			Fn_C_MTXPerspective,
			Fn_CameraMove__Fv,
			Fn_check__7cSatMgrP6cModelUi,
			Fn_Draw__8FILTER0C,
			Fn_EspgenMove__Fv,
			Fn_EspMove__Fv,
			Fn_ExePacket_SetParts__5EventP5Event,
			Fn_gameDiedemo__FP12DIEDEMO_WORK,
			Fn_memcpy,
			Fn_move__6cEmMgr,
			Fn_move__7cObjMgr,
			Fn_move__7Cockpit,
			Fn_move__7cPlayer,
			Fn_move__9cLightMgr,
			Fn_PadRead__Fv,
			Fn_ScenarioMove__Fv,
			Fn_Trans__14MessageControl,
			Fn_unitTrans__8IDSystemP7ID_UNIT;
	};
}
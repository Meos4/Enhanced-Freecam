#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	static constexpr Offset
	offsetPal
	{
		.pG = 0x00325588,
		.pPL = 0x00420650,
		.SubScreenWk = 0x00417800,
		.SystemSave = 0x004311E0,
		.Fn_C_MTXLookAt = 0x00252F00,
		.Fn_C_MTXPerspective = 0x00252FE8,
		.Fn_CameraMove__Fv = 0x001AD500,
		.Fn_check__7cSatMgrP6cModelUi = 0x001A19D0,
		.Fn_Draw__8FILTER0C = 0x002160C8,
		.Fn_EspgenMove__Fv = 0x001F6318,
		.Fn_EspMove__Fv = 0x001EA1A0,
		.Fn_ExePacket_SetParts__5EventP5Event = 0x00203A78,
		.Fn_gameDiedemo__FP12DIEDEMO_WORK = 0x0021A040,
		.Fn_memcpy = 0x002AF684,
		.Fn_move__6cEmMgr = 0x001BB078,
		.Fn_move__7cObjMgr = 0x002335B0,
		.Fn_move__7Cockpit = 0x001ADCA0,
		.Fn_move__7cPlayer = 0x00166DD0,
		.Fn_move__9cLightMgr = 0x00223768,
		.Fn_PadRead__Fv = 0x0023E7C8,
		.Fn_ScenarioMove__Fv = 0x00274478,
		.Fn_Trans__14MessageControl = 0x0012C320,
		.Fn_unitTrans__8IDSystemP7ID_UNIT = 0x0021E420,
	},
	offsetNtscU
	{
		.pG = 0x00325308,
		.pPL = 0x004203D0,
		.SubScreenWk = 0x00417580,
		.SystemSave = 0x00430F60,
		.Fn_C_MTXLookAt = 0x00252D88,
		.Fn_C_MTXPerspective = 0x00252E70,
		.Fn_CameraMove__Fv = 0x001AD418,
		.Fn_check__7cSatMgrP6cModelUi = 0x001A18E8,
		.Fn_Draw__8FILTER0C = 0x00215FE0,
		.Fn_EspgenMove__Fv = 0x001F6230,
		.Fn_EspMove__Fv = 0x001EA0B8,
		.Fn_ExePacket_SetParts__5EventP5Event = 0x00203990,
		.Fn_gameDiedemo__FP12DIEDEMO_WORK = 0x00219F58,
		.Fn_memcpy = 0x002AF3AC,
		.Fn_move__6cEmMgr = 0x001BAF90,
		.Fn_move__7cObjMgr = 0x00233438,
		.Fn_move__7Cockpit = 0x001ADBB8,
		.Fn_move__7cPlayer = 0x00166CE8,
		.Fn_move__9cLightMgr = 0x00223680,
		.Fn_PadRead__Fv = 0x0023E650,
		.Fn_ScenarioMove__Fv = 0x00274300,
		.Fn_Trans__14MessageControl = 0x0012C208,
		.Fn_unitTrans__8IDSystemP7ID_UNIT = 0x0021E338
	},
	offsetNtscJ
	{
		.pG = 0x00325F38,
		.pPL = 0x00421090,
		.SubScreenWk = 0x00418240,
		.SystemSave = 0x00431C20,
		.Fn_C_MTXLookAt = 0x00253828,
		.Fn_C_MTXPerspective = 0x00253910,
		.Fn_CameraMove__Fv = 0x001ADC08,
		.Fn_check__7cSatMgrP6cModelUi = 0x001A20D8,
		.Fn_Draw__8FILTER0C = 0x002169E0,
		.Fn_EspgenMove__Fv = 0x001F6C30,
		.Fn_EspMove__Fv = 0x001EAA88,
		.Fn_ExePacket_SetParts__5EventP5Event = 0x00204390,
		.Fn_gameDiedemo__FP12DIEDEMO_WORK = 0x0021A958,
		.Fn_memcpy = 0x002AFFAC,
		.Fn_move__6cEmMgr = 0x001BB778,
		.Fn_move__7cObjMgr = 0x00233EA8,
		.Fn_move__7Cockpit = 0x001AE3A8,
		.Fn_move__7cPlayer = 0x001674D8,
		.Fn_move__9cLightMgr = 0x002240F8,
		.Fn_PadRead__Fv = 0x0023F0F8,
		.Fn_ScenarioMove__Fv = 0x00274EB0,
		.Fn_Trans__14MessageControl = 0x0012C420,
		.Fn_unitTrans__8IDSystemP7ID_UNIT = 0x0021EDB0
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
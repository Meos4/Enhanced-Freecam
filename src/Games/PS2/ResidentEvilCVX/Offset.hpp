#pragma once

#include "Common/Types.hpp"

namespace PS2::ResidentEvilCVX
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 cam,
			DoorWrk,
			fNaViwClipNear,
			fVu1FarClip,
			plp,
			pNaMatMatrixStuckPtr,
			Ps2_zbuff_b,
			sysp,
			Fn__s2b,
			Fn_bhCheckWallEx,
			Fn_bhControlActiveCamera,
			Fn_bhControlCamera,
			Fn_bhControlDoor,
			Fn_bhControlEffect,
			Fn_bhControlEnemy,
			Fn_bhControlEvent,
			Fn_bhControlEventCamera,
			Fn_bhControlLight,
			Fn_bhControlMessage,
			Fn_bhControlObjTim,
			Fn_bhControlPlayer,
			Fn_bhDispFont,
			Fn_bhDrawCinesco,
			Fn_bhEff106, // Rain
			Fn_bhFixPosition,
			Fn_bhInitActiveCamera,
			Fn_bhInitCamera,
			Fn_bhSetCut,
			Fn_bhSetEventCamera,
			Fn_bhSysCallEvent,
			Fn_memcpy,
			Fn_njCalcFogPowerEx,
			Fn_njDrawPolygon2D,
			Fn_njGetMatrix,
			Fn_njTranslate,
			Fn_njUnitMatrix,
			Fn_Ps2_pad_read;
	};
}
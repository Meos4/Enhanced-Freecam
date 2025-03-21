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
			pNaMatMatrixStuckPtr,
			Ps2_zbuff_a,
			sysp,
			Fn_bhControlActiveCamera,
			Fn_bhControlCamera,
			Fn_bhControlDoor,
			Fn_bhControlEffect,
			Fn_bhControlEnemy,
			Fn_bhControlEvent,
			Fn_bhControlEventCamera,
			Fn_bhControlLight,
			Fn_bhControlObjTim,
			Fn_bhControlPlayer,
			Fn_bhDrawCinesco,
			Fn_bhEff106, // Rain
			Fn_bhInitActiveCamera,
			Fn_bhInitCamera,
			Fn_bhSetCut,
			Fn_bhSetEventCamera,
			Fn_bhSysCallEvent,
			Fn_njDrawPolygon2D,
			Fn_njGetMatrix,
			Fn_njTranslate,
			Fn_njUnitMatrix,
			Fn__s2b,
			Fn_memcpy,
			Fn_Ps2_pad_read;
	};
}
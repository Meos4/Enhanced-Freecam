#pragma once

#include "Common/Types.hpp"

namespace PS2::ResidentEvil4
{
	struct Offset final
	{
		static const Offset& create(s32 version);

		u32 Fn_memcpy,
			cameraPtr,
			menuStruct,
			Fn_drawHud,
			Fn_drawText,
			Fn_padStatus,
			Fn_setMatrix,
			Fn_transposeMatrix,
			Fn_unknown,
			Fn_unknown2,
			Fn_updateAction,
			Fn_updateExtern,
			Fn_updateGameOver,
			Fn_updateItems,
			Fn_updateNpc,
			Fn_updatePlayer,
			Fn_updateSfx,
			Fn_updateSfx2,
			Fn_updateTimer;
	};
}
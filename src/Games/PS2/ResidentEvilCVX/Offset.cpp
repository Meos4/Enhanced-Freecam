#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvilCVX
{
	static constexpr Offset
	offsetPal
	{
		.cam = 0x00449090,
		.DoorWrk = 0x0050F4C0,
		.fNaViwClipNear = 0x0034F5E0,
		.fVu1FarClip = 0x01148660,
		.pNaMatMatrixStuckPtr = 0x011486F0,
		.Ps2_zbuff_a = 0x01E2AB40,
		.sysp = 0x00449930,
		.Fn_bhControlActiveCamera = 0x00281750,
		.Fn_bhControlCamera = 0x0027EB40,
		.Fn_bhControlDoor = 0x002B4DE0,
		.Fn_bhControlEffect = 0x0021D250,
		.Fn_bhControlEnemy = 0x00175070,
		.Fn_bhControlEvent = 0x001560B0,
		.Fn_bhControlEventCamera = 0x00283500,
		.Fn_bhControlLight = 0x00285E60,
		.Fn_bhControlObjTim = 0x00287FF0,
		.Fn_bhControlPlayer = 0x0013ACC0,
		.Fn_bhDrawCinesco = 0x0026CBE0,
		.Fn_bhEff106 = 0x002300E0,
		.Fn_bhInitActiveCamera = 0x002804F0,
		.Fn_bhInitCamera = 0x0027EA90,
		.Fn_bhSetCut = 0x0027F7A0,
		.Fn_bhSetEventCamera = 0x002828B0,
		.Fn_bhSysCallEvent = 0x00133420,
		.Fn_njCalcFogPowerEx = 0x002DD900,
		.Fn_njDrawPolygon2D = 0x002E6A10,
		.Fn_njGetMatrix = 0x002DDFB0,
		.Fn_njTranslate = 0x002DE070,
		.Fn_njUnitMatrix = 0x002DDF00,
		.Fn__s2b = 0x00124450,
		.Fn_memcpy = 0x00123FD8,
		.Fn_Ps2_pad_read = 0x002E14B0
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetPal
		};

		return *offsets[version];
	}
}
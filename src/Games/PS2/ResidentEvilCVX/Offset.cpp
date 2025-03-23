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
		.plp = 0x0030C0A0,
		.pNaMatMatrixStuckPtr = 0x011486F0,
		.Ps2_zbuff_b = 0x01E2AB40,
		.sysp = 0x00449930,
		.Fn__s2b = 0x00124450,
		.Fn_bhCheckWallEx = 0x0025EF80,
		.Fn_bhControlActiveCamera = 0x00281750,
		.Fn_bhControlCamera = 0x0027EB40,
		.Fn_bhControlDoor = 0x002B4DE0,
		.Fn_bhControlEffect = 0x0021D250,
		.Fn_bhControlEnemy = 0x00175070,
		.Fn_bhControlEvent = 0x001560B0,
		.Fn_bhControlEventCamera = 0x00283500,
		.Fn_bhControlLight = 0x00285E60,
		.Fn_bhControlMessage = 0x002BD150,
		.Fn_bhControlObjTim = 0x00287FF0,
		.Fn_bhControlPlayer = 0x0013ACC0,
		.Fn_bhDispFont = 0x002BDD80,
		.Fn_bhDrawCinesco = 0x0026CBE0,
		.Fn_bhEff106 = 0x002300E0,
		.Fn_bhFixPosition = 0x00130D10,
		.Fn_bhInitActiveCamera = 0x002804F0,
		.Fn_bhInitCamera = 0x0027EA90,
		.Fn_bhSetCut = 0x0027F7A0,
		.Fn_bhSetEventCamera = 0x002828B0,
		.Fn_bhSysCallEvent = 0x00133420,
		.Fn_memcpy = 0x00123FD8,
		.Fn_njCalcFogPowerEx = 0x002DD900,
		.Fn_njDrawPolygon2D = 0x002E6A10,
		.Fn_njGetMatrix = 0x002DDFB0,
		.Fn_njTranslate = 0x002DE070,
		.Fn_njUnitMatrix = 0x002DDF00,
		.Fn_Ps2_pad_read = 0x002E14B0
	},
	offsetNtscU
	{
		.cam = 0x00432860,
		.DoorWrk = 0x004F78A0,
		.fNaViwClipNear = 0x00339C60,
		.fVu1FarClip = 0x01130A60,
		.plp = 0x00303CA0,
		.pNaMatMatrixStuckPtr = 0x01130AF0,
		.Ps2_zbuff_b = 0x01E12F40,
		.sysp = 0x00433100,
		.Fn__s2b = 0x00124450,
		.Fn_bhCheckWallEx = 0x0025E930,
		.Fn_bhControlActiveCamera = 0x0027CB60,
		.Fn_bhControlCamera = 0x00279F50,
		.Fn_bhControlDoor = 0x002AF370,
		.Fn_bhControlEffect = 0x0021CC70,
		.Fn_bhControlEnemy = 0x00174A90,
		.Fn_bhControlEvent = 0x00155AD0,
		.Fn_bhControlEventCamera = 0x0027E910,
		.Fn_bhControlLight = 0x00281270,
		.Fn_bhControlMessage = 0x002B7670,
		.Fn_bhControlObjTim = 0x00283400,
		.Fn_bhControlPlayer = 0x0013A6E0,
		.Fn_bhDispFont = 0x002B8190,
		.Fn_bhDrawCinesco = 0x0026C590,
		.Fn_bhEff106 = 0x0022FA90,
		.Fn_bhFixPosition = 0x00130D90,
		.Fn_bhInitActiveCamera = 0x0027B900,
		.Fn_bhInitCamera = 0x00279EA0,
		.Fn_bhSetCut = 0x0027ABB0,
		.Fn_bhSetEventCamera = 0x0027DCC0,
		.Fn_bhSysCallEvent = 0x001331A0,
		.Fn_memcpy = 0x00123FD8,
		.Fn_njCalcFogPowerEx = 0x002D63D0,
		.Fn_njDrawPolygon2D = 0x002DF450,
		.Fn_njGetMatrix = 0x002D6A80,
		.Fn_njTranslate = 0x002D6B40,
		.Fn_njUnitMatrix = 0x002D69D0,
		.Fn_Ps2_pad_read = 0x002D9F80
	},
	offsetNtscJ
	{
		.cam = 0x00430360,
		.DoorWrk = 0x005053E0,
		.fNaViwClipNear = 0x003378E0,
		.fVu1FarClip = 0x0113E560,
		.plp = 0x00302CA0,
		.pNaMatMatrixStuckPtr = 0x0113E5F0,
		.Ps2_zbuff_b = 0x01E209C0,
		.sysp = 0x00430C00,
		.Fn__s2b = 0x00124450,
		.Fn_bhCheckWallEx = 0x0025E000,
		.Fn_bhControlActiveCamera = 0x0027BD80,
		.Fn_bhControlCamera = 0x00279170,
		.Fn_bhControlDoor = 0x002AE8F0,
		.Fn_bhControlEffect = 0x0021C3D0,
		.Fn_bhControlEnemy = 0x00174200,
		.Fn_bhControlEvent = 0x00155320,
		.Fn_bhControlEventCamera = 0x0027DB30,
		.Fn_bhControlLight = 0x00280490,
		.Fn_bhControlMessage = 0x002B6C60,
		.Fn_bhControlObjTim = 0x00282620,
		.Fn_bhControlPlayer = 0x00139F30,
		.Fn_bhDispFont = 0x002B7750,
		.Fn_bhDrawCinesco = 0x0026BC60,
		.Fn_bhEff106 = 0x0022F1E0,
		.Fn_bhFixPosition = 0x00130D90,
		.Fn_bhInitActiveCamera = 0x0027AB20,
		.Fn_bhInitCamera = 0x002790C0,
		.Fn_bhSetCut = 0x00279DD0,
		.Fn_bhSetEventCamera = 0x0027CEE0,
		.Fn_bhSysCallEvent = 0x00133070,
		.Fn_memcpy = 0x00123FD8,
		.Fn_njCalcFogPowerEx = 0x002D5470,
		.Fn_njDrawPolygon2D = 0x002DE470,
		.Fn_njGetMatrix = 0x002D5B20,
		.Fn_njTranslate = 0x002D5BE0,
		.Fn_njUnitMatrix = 0x002D5A70,
		.Fn_Ps2_pad_read = 0x002D8F30
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
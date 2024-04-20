#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS1::ApeEscape
{
	static constexpr Offset
	offsetNtscU
	{
		.Fn_memcpy = 0x000A3760,
		.billboardMatrix = 0x000EBFD0,
		.cameraPosition = 0x000F45A8,
		.cutsceneState = 0x000F5AA0,
		.fog = 0x000EBFC8,
		.globalStruct = 0x000F4350,
		.minigame = 0x00100000,
		.overlay = 0x00136000,
		.projectionMatrix = 0x000AF0A0,
		.sonyLibrary = 0x000A8A80,
		.viewMatrix = 0x000E78C0,
		.Fn_drawActive = 0x00017CD4,
		.Fn_drawEnemy = 0x00051AA4,
		.Fn_drawLayer = 0x00016F34,
		.Fn_drawLayer2 = 0x00043A8C,
		.Fn_drawText = 0x0008D9D0,
		.Fn_inGameLoop = 0x0005BF70,
		.Fn_padStatus = 0x0001325C,
		.Fn_setViewMatrix = 0x00046670,
		.Fn_updateAdditional = 0x00050C34,
		.Fn_updateCollectable = 0x00084598,
		.Fn_updateMonkey = 0x00042E18,
		.Fn_updateMovable = 0x00088AB8
	},
	offsetNtscJ
	{
		.Fn_memcpy = 0x000A35A0,
		.billboardMatrix = 0x000EBBF0,
		.cameraPosition = 0x000F41C8,
		.cutsceneState = 0x000F56C0,
		.fog = 0x000EBBE8,
		.globalStruct = 0x000F3F70,
		.minigame = 0x00100000,
		.overlay = 0x00136000,
		.projectionMatrix = 0x000AEED0,
		.sonyLibrary = 0x000A88B0,
		.viewMatrix = 0x000E74E0,
		.Fn_drawActive = 0x00017CC4,
		.Fn_drawEnemy = 0x00051A94,
		.Fn_drawLayer = 0x00016F24,
		.Fn_drawLayer2 = 0x00043A7C,
		.Fn_drawText = 0x0008D7D0,
		.Fn_inGameLoop = 0x0005BF60,
		.Fn_padStatus = 0x0001324C,
		.Fn_setViewMatrix = 0x00046660,
		.Fn_updateAdditional = 0x00050C24,
		.Fn_updateCollectable = 0x00084398,
		.Fn_updateMonkey = 0x00042E08,
		.Fn_updateMovable = 0x000888B8
	},
	offsetNtscJRev1
	{
		.Fn_memcpy = 0x000A35F0,
		.billboardMatrix = 0x000EBC40,
		.cameraPosition = 0x000F4220,
		.cutsceneState = 0x000F5710,
		.fog = 0x000EBC38,
		.globalStruct = 0x000F3FC0,
		.minigame = 0x00100000,
		.overlay = 0x00136000,
		.projectionMatrix = 0x000AEF20,
		.sonyLibrary = 0x000A8900,
		.viewMatrix = 0x000E7530,
		.Fn_drawActive = 0x00017CC4,
		.Fn_drawEnemy = 0x00051A94,
		.Fn_drawLayer = 0x00016F24,
		.Fn_drawLayer2 = 0x00043A7C,
		.Fn_drawText = 0x0008D810,
		.Fn_inGameLoop = 0x0005BF60,
		.Fn_padStatus = 0x0001324C,
		.Fn_setViewMatrix = 0x00046660,
		.Fn_updateAdditional = 0x00050C24,
		.Fn_updateCollectable = 0x000843D8,
		.Fn_updateMonkey = 0x00042E08,
		.Fn_updateMovable = 0x000888F8
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetNtscU,
			&offsetNtscJ,
			&offsetNtscJRev1
		};

		return *offsets[version];
	}
}
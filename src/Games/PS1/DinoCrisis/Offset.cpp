#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS1::DinoCrisis
{
	static constexpr Offset
	offsetNtscU
	{
		.Fn_RotMatrix = 0x0008F8C0,
		.dinosaurCamera = 0x000C3230,
		.mainCamera = 0x000B7AD4,
		.drawFadeTbl = 0x0009CBE0,
		.Fn_dinosaurEntranceCameraTransition = 0x00027000,
		.Fn_drawEffects = 0x00047030,
		.Fn_drawFade = 0x00046C78,
		.Fn_moveNextCamera = 0x000264C4,
		.Fn_padStatus = 0x0001D9C8,
		.Fn_setCameraPosition = 0x0002669C,
		.Fn_setNextFrameCamera = 0x000269C8,
		.Fn_updateCamera = 0x00027210,
		.Fn_updateCharacters = 0x0004A3C4,
		.Fn_updateGameOver = 0x0002F0D4,
		.Fn_updateInteractable = 0x0004A320,
		.Fn_updateItems = 0x0004A274,
		.Fn_updateLinearCutsceneCamera = 0x0002753C,
		.Fn_updateMainCamera = 0x00072C2C,
		.Fn_updateMap = 0x000555E8,
		.Fn_updateSfx = 0x0004A460
	},
	offsetNtscU1_1
	{
		.Fn_RotMatrix = 0x0008F6F4,
		.dinosaurCamera = 0x000C3060,
		.mainCamera = 0x000B7904,
		.drawFadeTbl = 0x0009CA10,
		.Fn_dinosaurEntranceCameraTransition = 0x00026FFC,
		.Fn_drawEffects = 0x0004702C,
		.Fn_drawFade = 0x00046C74,
		.Fn_moveNextCamera = 0x000264C0,
		.Fn_padStatus = 0x0001D9C4,
		.Fn_setCameraPosition = 0x00026698,
		.Fn_setNextFrameCamera = 0x000269C4,
		.Fn_updateCamera = 0x0002720C,
		.Fn_updateCharacters = 0x0004A41C,
		.Fn_updateGameOver = 0x0002F0D0,
		.Fn_updateInteractable = 0x0004A378,
		.Fn_updateItems = 0x0004A2CC,
		.Fn_updateLinearCutsceneCamera = 0x00027538,
		.Fn_updateMainCamera = 0x00072C84,
		.Fn_updateMap = 0x00055640,
		.Fn_updateSfx = 0x0004A4B8
	},
	offsetNtscJ
	{
		.Fn_RotMatrix = 0x00087394,
		.dinosaurCamera = 0x000B7E98,
		.mainCamera = 0x000AC73C,
		.drawFadeTbl = 0x00094614,
		.Fn_dinosaurEntranceCameraTransition = 0x0001EEF0,
		.Fn_drawEffects = 0x0003EF20,
		.Fn_drawFade = 0x0003EB68,
		.Fn_moveNextCamera = 0x0001E3B4,
		.Fn_padStatus = 0x000159FC,
		.Fn_setCameraPosition = 0x0001E58C,
		.Fn_setNextFrameCamera = 0x0001E8B8,
		.Fn_updateCamera = 0x0001F100,
		.Fn_updateCharacters = 0x000423B0,
		.Fn_updateGameOver = 0x00026FC4,
		.Fn_updateInteractable = 0x0004230C,
		.Fn_updateItems = 0x00042260,
		.Fn_updateLinearCutsceneCamera = 0x0001F42C,
		.Fn_updateMainCamera = 0x0006A874,
		.Fn_updateMap = 0x0004D574,
		.Fn_updateSfx = 0x0004244C
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetNtscU,
			&offsetNtscU1_1,
			&offsetNtscJ
		};

		return *offsets[version];
	}
}
#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DragonQuest8
{
	static constexpr Offset
	offsetPal
	{
		.Fn_CreatePose__7CCameraFv = 0x001677E0,
		.Fn_DrawStep__12CModelObjectFf = 0x001EA450,
		.Fn_mgAddVector__FPfPf = 0x0013DF80,
		.Fn_Roll__9mgCCameraFf = 0x001669E0,
		.Fn_SetPos__7CCameraFPf = 0x00167020,
		.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo = 0x00156D00,
		.Fn_Step__14CCameraControlFf = 0x00170310,
		.Fn_TimeStep__8dqCSceneFf = 0x001886B0,
		.alchemyPtr = 0x003E0B30,
		.bag = 0x00409DD0,
		.cameraPtr = 0x003E0420,
		.cutsceneState = 0x003DFC28,
		.fov = 0x003EEE04,
		.gameState = 0x003FF744,
		.heroStats = 0x00409B70,
		.projectionStruct = 0x003EEDF0,
		.timer = 0x00401DA0,
		.walkSpeed = 0x003DF6DC,
		.Fn_drawBattleHud = 0x002D7B00,
		.Fn_drawCommonHud = 0x001770C0,
		.Fn_drawCompass = 0x0021D1F0,
		.Fn_drawMainCharacter = 0x00178A20,
		.Fn_drawMidTextOnScreen = 0x0036D1F0,
		.Fn_drawSkybox = 0x0019C9D0,
		.Fn_drawTextOnScreen = 0x002E4160,
		.Fn_npcScript = 0x001D6D60,
		.Fn_npcStep = 0x0016D320,
		.Fn_nullsub1 = 0x00100320,
		.Fn_padStatus = 0x00164EB0,
		.Fn_playerStep = 0x001DE720,
		.Fn_randomEncounterBehavior = 0x00302360,
		.Fn_updateBattle = 0x002AB0A0
	},
	offsetNtscU
	{
		.Fn_CreatePose__7CCameraFv = 0x001687E0,
		.Fn_DrawStep__12CModelObjectFf = 0x001E9700,
		.Fn_mgAddVector__FPfPf = 0x0013F640,
		.Fn_Roll__9mgCCameraFf = 0x001679E0,
		.Fn_SetPos__7CCameraFPf = 0x00168020,
		.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo = 0x00157CE0,
		.Fn_Step__14CCameraControlFf = 0x00171220,
		.Fn_TimeStep__8dqCSceneFf = 0x00189290,
		.alchemyPtr = 0x003D2F18,
		.bag = 0x00409AD0,
		.cameraPtr = 0x003D2814,
		.cutsceneState = 0x003D20A8,
		.fov = 0x003E57C0,
		.gameState = 0x003F5F24,
		.heroStats = 0x00409870,
		.projectionStruct = 0x003E57B0,
		.timer = 0x003F8570,
		.walkSpeed = 0x003D1BCC,
		.Fn_drawBattleHud = 0x002D6360,
		.Fn_drawCommonHud = 0x00177E90,
		.Fn_drawCompass = 0x0021C4F0,
		.Fn_drawMainCharacter = 0x00179960,
		.Fn_drawMidTextOnScreen = 0x00364580,
		.Fn_drawSkybox = 0x0019D5D0,
		.Fn_drawTextOnScreen = 0x002E3A40,
		.Fn_npcScript = 0x001D6710,
		.Fn_npcStep = 0x0016E220,
		.Fn_nullsub1 = 0x00100320,
		.Fn_padStatus = 0x00165FA0,
		.Fn_playerStep = 0x001DE0C0,
		.Fn_randomEncounterBehavior = 0x002FA750,
		.Fn_updateBattle = 0x002A9E20
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetPal,
			&offsetNtscU
		};

		return *offsets[version];
	}
}
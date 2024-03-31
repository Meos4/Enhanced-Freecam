#include "Offset.hpp"

#include "Game.hpp"

#include <array>

namespace PS1::DinoCrisis
{
	static constexpr Offset
	offsetNtscU1_1
	{
		.Fn_RotMatrix = 0x0008F6F4,
		.dinosaurCamera = 0x000C3060,
		.mainCamera = 0x000B7904,
		.Fn_dinosaurEntranceCameraTransition = 0x00026FFC,
		.Fn_drawEffects = 0x0004702C,
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
	};

	const Offset& Offset::create(s32 version)
	{
		static constexpr std::array<const Offset*, Version::Count> offsets
		{
			&offsetNtscU1_1
		};

		return *offsets[version];
	}
}
#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::Sly1::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SCES-50917]",
			"NTSC-U - [SCUS-97198]",
			"NTSC-J - [SCPS-15036]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x0024D76B, { 0, 96, 192, 24, 0, 216, 193, 24, 0, 96, 192, 24, 0, 88, 193, 24, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 47, 66, 69, 83, 67, 69, 83, 45, 53, 48, 57, 49, 55, 89, 65, 79, 84, 87, 84, 68, 33, 0, 0, 0, 47, 66, 69, 83, 67, 69, 83, 45, 53, 48, 57 },
			0x0024B32B, { 0, 248, 185, 24, 0, 112, 187, 24, 0, 248, 185, 24, 0, 240, 186, 24, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 47, 66, 65, 83, 67, 85, 83, 45, 57, 55, 49, 57, 56, 89, 65, 79, 84, 87, 84, 68, 33, 0, 0, 0, 47, 66, 65, 83, 67, 85, 83, 45, 57, 55, 49 },
			0x0024C93B, { 0, 0, 195, 24, 0, 120, 196, 24, 0, 0, 195, 24, 0, 248, 195, 24, 0, 0, 0, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 47, 66, 73, 83, 67, 80, 83, 45, 49, 53, 48, 51, 54, 89, 65, 79, 84, 87, 84, 68, 33, 0, 0, 0, 47, 66, 73, 83, 67, 80, 83, 45, 49, 53, 48 }
		};

		return { vOp[version].offset, vOp[version].pattern };
	}

	std::unique_ptr<GameLoop> createLoop(Ram&& ram, s32 version)
	{
		return std::make_unique<Loop>(std::move(ram), version);
	}

	std::vector<InputWrapper::NameInputs> baseInputs()
	{
		const auto& i{ g_settings.input };

		return
		{
			{ "Toggle Freecam", i.toggleFreecam },
			{ "Move Forward", i.moveForward },
			{ "Move Backward", i.moveBackward },
			{ "Move Right", i.moveRight },
			{ "Move Left", i.moveLeft },
			{ "Move Up", i.moveUp },
			{ "Move Down", i.moveDown },
			{ "Rotate X+", i.rotateCounterclockwise },
			{ "Rotate X-", i.rotateClockwise },
			{ "Rotate Y+", i.rotateUp },
			{ "Rotate Y-", i.rotateDown },
			{ "Rotate Z+", i.rotateRight },
			{ "Rotate Z-", i.rotateLeft },
			{ "Fov+", i.fovIncrease },
			{ "Fov-", i.fovDecrease },
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud },
			{ "Button", i.button },
			{ "Joystick", i.joystick },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Fov Speed+", i.fovSpeedPos },
			{ "Fov Speed-", i.fovSpeedNeg },
			{ "Teleport To Camera", i.teleportToCamera }
		};
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"DA3DD765", 0x00100000, 0x002132C0,
			"C77AF2CA", 0x00100000, 0x00211E00,
			"15C88C7B", 0x00100000, 0x002135E0
		};

		return pnachInfos[version];
	}
}
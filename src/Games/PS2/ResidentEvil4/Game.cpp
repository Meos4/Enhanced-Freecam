#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::ResidentEvil4::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-53702]",
			"NTSC-U - [SLUS-21134]",
			"NTSC-J - [SLPM-66213]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x003AE938, { 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 148, 155, 22, 0, 64, 155, 22, 0, 0, 0, 0, 0, 66, 69, 83, 76, 69, 83, 45, 53, 51, 55, 48, 50, 83, 89, 83, 0, 98, 105, 111, 52, 46, 105, 99, 111 },
			0x003AE6B8, { 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 172, 154, 22, 0, 88, 154, 22, 0, 0, 0, 0, 0, 66, 65, 83, 76, 85, 83, 45, 50, 49, 49, 51, 52, 83, 89, 83, 0, 98, 105, 111, 52, 46, 105, 99, 111 },
			0x003AF2B8, { 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 88, 162, 22, 0, 0, 0, 0, 0, 66, 73, 83, 76, 80, 77, 45, 54, 54, 50, 49, 51, 83, 89, 83, 0, 98, 105, 111, 52, 46, 105, 99, 111 }
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
			{ "Rotate X+", i.rotateUp },
			{ "Rotate X-", i.rotateDown },
			{ "Rotate Y+", i.rotateLeft },
			{ "Rotate Y-", i.rotateRight },
			{ "Rotate Z+", i.rotateCounterclockwise },
			{ "Rotate Z-", i.rotateClockwise },
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
			"6BA2F6B9", 0x00100000, 0x002EFBE8,
			"013E349D", 0x00100000, 0x002EF918,
			"60FA8C69", 0x00100000, 0x002F0518
		};

		return pnachInfos[version];
	}
}
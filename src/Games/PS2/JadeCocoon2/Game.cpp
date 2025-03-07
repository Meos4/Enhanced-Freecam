#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::JadeCocoon2::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-50735]",
			"NTSC-U - [SLUS-20309]",
			"NTSC-J - [SLPS-25054]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00662DF0, { 16, 79, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 79, 79, 84, 50, 32, 61, 32, 99, 100, 114, 111, 109, 48, 58, 92, 83, 76, 69, 83, 95, 53, 48, 55, 46, 51, 53, 59, 49, 10, 86, 69, 82, 32, 61, 32, 49, 46, 48, 51, 10, 86, 77, 79, 68, 69, 32, 61 },
			0x00662890, { 48, 75, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 79, 79, 84, 50, 32, 61, 32, 99, 100, 114, 111, 109, 48, 58, 92, 83, 76, 85, 83, 95, 50, 48, 51, 46, 48, 57, 59, 49, 10, 86, 69, 82, 32, 61, 32, 49, 46, 48, 49, 10, 86, 77, 79, 68, 69, 32, 61 },
			0x0065EB70, { 160, 32, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 79, 79, 84, 50, 32, 61, 32, 99, 100, 114, 111, 109, 48, 58, 92, 83, 76, 80, 83, 95, 50, 53, 48, 46, 53, 52, 59, 49, 10, 86, 69, 82, 32, 61, 32, 49, 46, 48, 56, 10, 86, 77, 79, 68, 69, 32, 61 }
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
			{ "Rotate Y+", i.rotateRight },
			{ "Rotate Y-", i.rotateLeft },
			{ "Rotate Z+", i.rotateClockwise },
			{ "Rotate Z-", i.rotateCounterclockwise },
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
			{ "Fov Speed-", i.fovSpeedNeg }
		};
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"7FD7A1B9", 0x00100000, 0x004D8CCC,
			"CA91718B", 0x00100000, 0x004D879C,
			"CA067714", 0x00100000, 0x004D51A4
		};

		return pnachInfos[version];
	}
}
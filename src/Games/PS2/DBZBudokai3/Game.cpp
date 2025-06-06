#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::DBZBudokai3::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-52730]",
			"PAL Collector - [SLES-53346]",
			"NTSC-U - [SLUS-20998]",
			"NTSC-U GH - [SLUS-20998]",
			"NTSC-J - [SLPS-25460]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x0046F0E8, { 83, 79, 70, 68, 69, 67, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 50, 55, 51, 48, 68, 66, 90, 51, 47, 66, 69, 83, 76, 69, 83, 45, 53, 50, 55, 51, 48, 68, 66, 90, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45 },
			0x00427E78, { 83, 79, 70, 68, 69, 67, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 51, 51, 52, 54, 68, 66, 90, 51, 47, 66, 69, 83, 76, 69, 83, 45, 53, 51, 51, 52, 54, 68, 66, 90, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45 },
			0x0046B8E8, { 83, 79, 70, 68, 69, 67, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 48, 57, 57, 56, 68, 66, 90, 51, 47, 66, 65, 83, 76, 85, 83, 45, 50, 48, 57, 57, 56, 68, 66, 90, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45 },
			0x00422E98, { 83, 79, 70, 68, 69, 67, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 48, 57, 57, 56, 68, 66, 90, 51, 71, 72, 47, 66, 65, 83, 76, 85, 83, 45, 50, 48, 57, 57, 56, 68, 66, 90, 51, 71, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45 },
			0x00425E08, { 83, 79, 70, 68, 69, 67, 0, 0, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 52, 54, 48, 68, 66, 90, 51, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 52, 54, 48, 68, 66, 90, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 73, 83, 76, 80, 83, 45 }
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
			{ "Timescale+", i.timescalePos },
			{ "Timescale-", i.timescaleNeg },
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

	std::span<const char* const> stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Battle",
			"Dragon Universe"
		};

		return names;
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"CD787D68", 0x00100000, 0x003144B4,
			"4E0D7BDE", 0x00100000, 0x00326940,
			"2A4B60EB", 0x00100000, 0x00310900,
			"C97EF0A4", 0x00100000, 0x003218C0,
			"5B26AFF9", 0x00100000, 0x00324BA4
		};

		return pnachInfos[version];
	}
}
#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::DBZInfiniteWorld::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-55347]",
			"NTSC-U - [SLUS-21842]",
			"NTSC-J - [SLPS-25905]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00497B73, { 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 47, 66, 69, 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 47 },
			0x00497373, { 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 47 },
			0x00497AD3, { 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 47 }
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
			"Dragon Mission",
			"Dragon Mission (Flying)",
			"Dragon Mission (Cutscene)"
		};

		return names;
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"335A5A1F", 0x00100000, 0x003826EC,
			"E90DC5C9", 0x00100000, 0x00381FC4,
			"C7E583CF", 0x00100000, 0x00382614
		};

		return pnachInfos[version];
	}
}
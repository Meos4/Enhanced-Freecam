#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::DragonQuest8::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-53974]",
			"NTSC-U - [SLUS-21207]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x003A5411, { 79, 242, 111, 243, 111, 244, 111, 245, 111, 246, 111, 217, 85, 218, 85, 219, 85, 220, 85, 249, 117, 250, 117, 251, 117, 252, 117, 223, 115, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 51, 57, 55, 52, 100, 113, 56, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			0x003990A1, { 79, 242, 111, 243, 111, 244, 111, 245, 111, 246, 111, 217, 85, 218, 85, 219, 85, 220, 85, 249, 117, 250, 117, 251, 117, 252, 117, 223, 115, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 50, 48, 55, 100, 113, 56, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
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
			{ "Fov Speed-", i.fovSpeedNeg }
		};
	}

	std::span<const char* const> stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Common",
			"Cutscene",
			"Companions",
			"Alchemy / Mini map",
			"Battle"
		};

		return names;
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"945FBF31", 0x00100000, 0x0038DA08,
			"F4715852", 0x00100000, 0x0038183C
		};

	 	return pnachInfos[version];
	}
}
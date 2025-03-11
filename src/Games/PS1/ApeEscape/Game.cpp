#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS1::ApeEscape::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"NTSC-U - [SCUS-94423]",
			"NTSC-J - [SCPS-10091]",
			"NTSC-J rev 1 - [SCPS-10091]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00098330, { 14, 128, 2, 60, 192, 7, 67, 36, 192, 7, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 96, 0, 5, 36, 4, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36, 8, 0, 98, 164 },
			0x00098170, { 14, 128, 2, 60, 224, 3, 67, 36, 224, 3, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 4, 0, 98, 160, 112, 0, 2, 36, 5, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36 },
			0x000981C0, { 14, 128, 2, 60, 48, 4, 67, 36, 48, 4, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 96, 0, 5, 36, 4, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36, 8, 0, 98, 164 }
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
			{ "Rotate X+", i.rotateDown },
			{ "Rotate X-", i.rotateUp },
			{ "Rotate Y+", i.rotateLeft },
			{ "Rotate Y-", i.rotateRight },
			{ "Rotate Z+", i.rotateCounterclockwise },
			{ "Rotate Z-", i.rotateClockwise },
			{ "Fov+", i.fovDecrease },
			{ "Fov-", i.fovIncrease },
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud },
			{ "Button", i.button },
			{ "L Joystick", i.lJoystick },
			{ "R Joystick", i.rJoystick },
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
			"Common (Cutscene)",
			"Pre-Rendered Cutscene",
			"Title Screen",
			"Cutscene",
			"Stage Select",
			"Space Menu",
			"Training Preview",
			"Stage Preview",
			"Clear Stage",
			"Race Result",
			"Ski Kidz Racing",
			"Specter Boxing",
			"Galaxy Monkey"
		};

		return names;
	}
}
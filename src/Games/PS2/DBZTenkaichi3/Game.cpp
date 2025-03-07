#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::DBZTenkaichi3::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-54945]",
			"NTSC-U - [SLUS-21678]",
			"NTSC-J - [SLPS-25815]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x002F45A0, { 92, 83, 76, 69, 83, 95, 53, 51, 50, 46, 48, 48, 59, 49, 0, 0, 160, 85, 38, 0, 168, 85, 38, 0, 176, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 184, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 192, 85, 38, 0, 208, 85, 38, 0 },
			0x002F34F0, { 92, 83, 76, 85, 83, 95, 50, 49, 50, 46, 50, 55, 59, 49, 0, 0, 0, 79, 38, 0, 8, 79, 38, 0, 16, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 24, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 32, 79, 38, 0, 48, 79, 38, 0 },
			0x002F34F0, { 92, 83, 76, 80, 83, 95, 50, 53, 53, 46, 54, 48, 59, 49, 0, 0, 16, 77, 38, 0, 24, 77, 38, 0, 32, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 40, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 48, 77, 38, 0, 64, 77, 38, 0 }
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
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud },
			{ "Button", i.button },
			{ "Joystick", i.joystick },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg }
		};
	}

	std::span<const char* const> stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Battle",
			"Battle Cutscene",
			"Dragon History",
			"View Model",
			"Shenron Cutscene",
			"Shenron Wish"
		};

		return names;
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"A422BB13", 0x00100000, 0x002C067C,
			"428113C2", 0x00100000, 0x002BF6AC,
			"F28D21F1", 0x00100000, 0x002BF5A4
		};

		return pnachInfos[version];
	}
}
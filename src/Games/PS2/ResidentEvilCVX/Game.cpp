#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <type_traits>

namespace PS2::ResidentEvilCVX::Game
{
	const char* versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-50306]",
			"NTSC-U - [SLUS-20184]"
		};

		return vText[version];
	}

	OffsetPattern offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00335518, { 64, 84, 51, 0, 80, 84, 51, 0, 96, 84, 51, 0, 112, 84, 51, 0, 128, 84, 51, 0, 144, 84, 51, 0, 160, 84, 51, 0, 176, 84, 51, 0, 192, 84, 51, 0, 208, 84, 51, 0, 224, 84, 51, 0, 240, 84, 51, 0, 66, 69, 83, 76, 69, 83, 45, 53, 48, 51, 48, 54, 0, 0, 0, 0 },
			0x0032D108, { 48, 208, 50, 0, 64, 208, 50, 0, 80, 208, 50, 0, 96, 208, 50, 0, 112, 208, 50, 0, 128, 208, 50, 0, 144, 208, 50, 0, 160, 208, 50, 0, 176, 208, 50, 0, 192, 208, 50, 0, 208, 208, 50, 0, 224, 208, 50, 0, 66, 65, 83, 76, 85, 83, 45, 50, 48, 49, 56, 52, 0, 0, 0, 0 }
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
			{ "Fov+", i.fovIncrease },
			{ "Fov-", i.fovDecrease },
			{ "Pause Game", i.pauseGame },
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

	std::span<const char* const> stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Common",
			"Door"
		};

		return names;
	}

	const PCSX2::PnachInfo& pnachInfo(s32 version)
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"6EA9DDA9", 0x00100000, 0x0030A320,
			"24036809", 0x00100000, 0x00301F10
		};

		return pnachInfos[version];
	}
}
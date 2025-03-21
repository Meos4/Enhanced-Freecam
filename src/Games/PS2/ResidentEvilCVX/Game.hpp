#pragma once

#include "Common/PS2/PCSX2.hpp"

#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <memory>
#include <span>
#include <vector>

namespace PS2::ResidentEvilCVX
{
	struct Input
	{
		enum
		{
			ToggleFreecam,
			MoveForward,
			MoveBackward,
			MoveRight,
			MoveLeft,
			MoveUp,
			MoveDown,
			RotateXPos,
			RotateXNeg,
			RotateYPos,
			RotateYNeg,
			RotateZPos,
			RotateZNeg,
			FovPos,
			FovNeg,
			PauseGame,
			Button,
			Joystick,
			MovementSpeedPos,
			MovementSpeedNeg,
			RotationSpeedPos,
			RotationSpeedNeg,
			FovSpeedPos,
			FovSpeedNeg,
			Count
		};
	};

	struct Version
	{
		enum
		{
			Pal,
			Count
		};
	};

	struct State
	{
		enum
		{
			None = -1,
			Common,
			Door,
			Count
		};
	};

	namespace Game
	{
		inline constexpr auto name{ "Resident Evil - CODE Veronica X" };

		const char* versionText(s32 version);
		OffsetPattern offsetPattern(s32 version);
		std::unique_ptr<GameLoop> createLoop(Ram&& ram, s32 version);
		std::vector<InputWrapper::NameInputs> baseInputs();
		std::span<const char* const> stateNames();
		const PCSX2::PnachInfo& pnachInfo(s32 version);
	};
}
#pragma once

#include "Common/PS2/PCSX2.hpp"

#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <memory>
#include <vector>

namespace PS2::ResidentEvil4
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
			HideHud,
			Button,
			Joystick,
			MovementSpeedPos,
			MovementSpeedNeg,
			RotationSpeedPos,
			RotationSpeedNeg,
			FovSpeedPos,
			FovSpeedNeg,
			TeleportToCamera,
			Count
		};
	};

	struct Version
	{
		enum
		{
			Pal,
			NtscU,
			NtscJ,
			Count
		};
	};

	namespace Game
	{
		inline constexpr auto name{ "Resident Evil 4" };

		const char* versionText(s32 version);
		OffsetPattern offsetPattern(s32 version);
		std::unique_ptr<GameLoop> createLoop(Ram&& ram, s32 version);
		std::vector<InputWrapper::NameInputs> baseInputs();
		const PCSX2::PnachInfo& pnachInfo(s32 version);
	};
}
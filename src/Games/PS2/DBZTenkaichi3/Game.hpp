#pragma once

#include "Common/PS2/PCSX2.hpp"

#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <memory>
#include <span>
#include <vector>

namespace PS2::DBZTenkaichi3
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
			PauseGame,
			HideHud,
			Button,
			Joystick,
			MovementSpeedPos,
			MovementSpeedNeg,
			RotationSpeedPos,
			RotationSpeedNeg,
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

	struct State
	{
		enum
		{
			None = -1,
			Battle,
			BattleCutscene,
			DragonHistory,
			ViewModel,
			ShenronCutscene,
			ShenronWish,
			Count
		};
	};

	namespace Game
	{
		inline constexpr auto name{ "Dragon Ball Z - Budokai Tenkaichi 3" };

		const char* versionText(s32 version);
		OffsetPattern offsetPattern(s32 version);
		std::unique_ptr<GameLoop> createLoop(Ram&& ram, s32 version);
		std::vector<InputWrapper::NameInputs> baseInputs();
		std::span<const char* const> stateNames();
		const PCSX2::PnachInfo& pnachInfo(s32 version);
	};
}
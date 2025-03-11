#pragma once

#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include <memory>
#include <span>
#include <vector>

namespace PS1::ApeEscape
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
			LJoystick,
			RJoystick,
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
			NtscU,
			NtscJ,
			NtscJRev1,
			Count
		};
	};

	struct State
	{
		enum
		{
			None = -1,
			Ingame,
			IngameCutscene,
			AllVideo,
			TitleScreen,
			Cutscene,
			StageSelect,
			SpaceMenu,
			TrainingPreview,
			StagePreview,
			ClearStage,
			RaceResult,
			SkiKidzRacing,
			SpecterBoxing,
			GalaxyMonkey,
			Count
		};
	};

	namespace Game
	{
		inline constexpr auto name{ "Ape Escape" };

		const char* versionText(s32 version);
		OffsetPattern offsetPattern(s32 version);
		std::unique_ptr<GameLoop> createLoop(Ram&& ram, s32 version);
		std::vector<InputWrapper::NameInputs> baseInputs();
		std::span<const char* const> stateNames();
	};
}
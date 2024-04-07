#pragma once

#include "Common/PS2/PCSX2.hpp"

#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Json.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"

#include "Offset.hpp"
#include "Settings.hpp"

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
			MovementSpeedPos,
			MovementSpeedNeg,
			RotationSpeedPos,
			RotationSpeedNeg,
			PauseGame,
			HideHud,
			TimescalePos,
			TimescaleNeg,
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
			Battle,
			BattleCutscene,
			DragonHistory,
			ViewModel,
			ShenronCutscene,
			ShenronWish,
			Count
		};
	};

	class Game final
	{
	public:
		Game(Ram ram, s32 version);

		static constexpr auto name{ "Dragon Ball Z - Budokai Tenkaichi 3" };

		static const char* versionText(s32 version);
		static OffsetPattern offsetPattern(s32 version);
		static std::unique_ptr<GameLoop> createLoop(Ram ram, s32 version);
		static std::vector<InputWrapper::NameInputs> baseInputs();
		static std::span<const char* const> stateNames();

		void update();
		const PCSX2::PnachInfo& pnachInfo() const;
		void readSettings(const Json::Read& json);
		void writeSettings(Json::Write* json);

		const Ram& ram() const;
		s32 version() const;
		const Offset& offset() const;
		Settings* settings();
		InputWrapper* input();
		s32 state() const;
	private:
		Ram m_ram;
		s32 m_version;
		const Offset& m_offset;
		Settings m_settings;
		InputWrapper m_input;
		s32 m_state{ State::None };
	};
}
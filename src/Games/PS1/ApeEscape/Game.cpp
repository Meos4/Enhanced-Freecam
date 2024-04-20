#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>
#include <cstring>
#include <type_traits>

namespace PS1::ApeEscape
{
	Game::Game(Ram ram, s32 version)
		: m_ram(ram), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"NTSC-U - [SCUS-94423]",
			"NTSC-J - [SCPS-10091]",
			"NTSC-J rev 1 - [SCPS-10091]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00098330, { 14, 128, 2, 60, 192, 7, 67, 36, 192, 7, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 96, 0, 5, 36, 4, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36, 8, 0, 98, 164 },
			0x00098170, { 14, 128, 2, 60, 224, 3, 67, 36, 224, 3, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 4, 0, 98, 160, 112, 0, 2, 36, 5, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36 },
			0x000981C0, { 14, 128, 2, 60, 48, 4, 67, 36, 48, 4, 68, 160, 16, 0, 2, 36, 1, 0, 98, 160, 250, 0, 2, 36, 2, 0, 98, 164, 254, 255, 130, 36, 64, 17, 2, 0, 96, 0, 5, 36, 4, 0, 98, 160, 31, 0, 2, 36, 6, 0, 98, 160, 7, 0, 98, 160, 173, 125, 2, 36, 8, 0, 98, 164 }
		};

		return { vOp[version].offset, vOp[version].pattern };
	}

	std::unique_ptr<GameLoop> Game::createLoop(Ram ram, s32 version)
	{
		return std::make_unique<Loop>(Game{ ram, version });
	}

	std::vector<InputWrapper::NameInputs> Game::baseInputs()
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
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud }
		};
	}

	std::span<const char* const> Game::stateNames()
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
			"Specter Boxing"
		};

		return names;
	}

	void Game::update()
	{
		// Loop Table Id (Mainly overlays from KKIIDDZZ.BNS but not only)
		enum : u16
		{
			LOOP_SONY_INTRO = 0,
			LOOP_SONY_INTRO_2 = 1,
			LOOP_INIT_SPECTER_BOXING = 2,
			LOOP_TITLE_SCREEN = 3,
			LOOP_RETURN_0 = 4,
			LOOP_SOUND_TEST_DEBUG = 5,
			LOOP_MINI_GAME_DEBUG = 6,
			LOOP_STAGE_SELECT_DEBUG = 7,
			LOOP_CUTSCENE = 8,
			LOOP_STAGE_SELECT = 9,
			LOOP_STAGE_PREVIEW = 10,
			LOOP_INGAME_STAGE = 11,
			LOOP_CLEAR_STAGE = 12,
			LOOP_INGAME_TIME_STATION = 13,
			LOOP_SAVE_LOAD = 14,
			LOOP_GAME_OVER = 15,
			LOOP_INIT_TRAINING_PREVIEW = 16,
			LOOP_TRAINING_PREVIEW = 17,
			LOOP_STAGE_PREVIEW_TIME_ATTACK = 18,
			LOOP_INGAME_STAGE_TIME_ATTACK = 19,
			LOOP_CLEAR_STAGE_TIME_ATTACK = 20,
			LOOP_SPACE_MENU = 21,
			LOOP_POCKETSTATION = 22,
			// 23
			LOOP_INGAME_UNKNOWN = 24,
			LOOP_RACE_RESULT = 25,
			LOOP_ALL_VIDEO = 26,
			LOOP_DEMO_STR_SAMPLE_IKI = 27,
			LOOP_MONKEY_BOOK = 28,
			LOOP_CREDITS = 29,
			LOOP_CREDITS_100 = 30,
			LOOP_END_DEMO_DISC_TALK = 31,
			LOOP_END_DEMO_DISC_PREVIEW = 32,
			LOOP_GALAXY_MONKEY = 33,
			LOOP_QUIT_DEMO_DISC = 34,
			LOOP_SAVE_CONFIRM = 35,
			LOOP_TITLE_SCREEN_DEMO = 36
		};

		// Ski Kidz Racing (MINI1.EXE) | Specter Boxing (MINI2.EXE)
		static constexpr std::array<u8, 32> 
		MINI1Begin
		{
			83, 75, 82, 69, 70, 46, 66, 73, 78, 59, 49, 0, 38, 2, 88, 2, 138, 2, 188, 2, 82, 3, 8, 2, 244, 1, 88, 2, 38, 2, 8, 2
		},
		MINI2Begin
		{
			140, 13, 16, 128, 112, 14, 16, 128, 148, 15, 16, 128, 136, 16, 16, 128, 180, 17, 16, 128, 168, 18, 16, 128, 204, 19, 16, 128, 192, 20, 16, 128
		};

		std::remove_const_t<decltype(MINI1Begin)> bufferMg;
		auto* const bufferMgPtr{ bufferMg.data() };
		m_ram.read(m_offset.minigame, bufferMgPtr, bufferMg.size());

		if (std::memcmp(bufferMgPtr, MINI1Begin.data(), bufferMg.size()) == 0)
		{
			m_state = State::SkiKidzRacing;
			return;
		}
		if (std::memcmp(bufferMgPtr, MINI2Begin.data(), bufferMg.size()) == 0)
		{
			m_state = State::SpecterBoxing;
			return;
		}

		if (m_ram.read<u8>(m_offset.cutsceneState) == 1)
		{
			m_state = State::IngameCutscene;
			return;
		}

		u32 loopIndexShift;

		if (m_version == Version::NtscU || m_version == Version::NtscJ)
		{
			loopIndexShift = 0x120;
		}
		else
		{
			loopIndexShift = 0x128;
		}

		const auto loopIndex{ m_ram.read<u16>(m_offset.globalStruct + loopIndexShift) };
		
		if (loopIndex == LOOP_TRAINING_PREVIEW && m_ram.read<u16>(m_offset.fog) != 4096)
		{
			switch (m_ram.read<u16>(m_offset.globalStruct + loopIndexShift + 6))
			{
			case 92:
			case 93:
			case 94:
			case 95:
			case 96:
			case 97:
			case 98:
				m_state = State::Ingame;
				return;
			}
		}

		switch (loopIndex)
		{
		case LOOP_ALL_VIDEO: m_state = State::AllVideo; return;
		case LOOP_TITLE_SCREEN: m_state = State::TitleScreen; return;
		case LOOP_CUTSCENE: m_state = State::Cutscene; return;

		case LOOP_INGAME_STAGE:
		case LOOP_INGAME_TIME_STATION:
		case LOOP_INGAME_STAGE_TIME_ATTACK:
		case LOOP_TITLE_SCREEN_DEMO:
			m_state = State::Ingame; return;

		case LOOP_STAGE_SELECT: m_state = State::StageSelect; return;
		case LOOP_SPACE_MENU: m_state = State::SpaceMenu; return;
		case LOOP_TRAINING_PREVIEW: m_state = State::TrainingPreview; return;

		case LOOP_STAGE_PREVIEW:
		case LOOP_STAGE_PREVIEW_TIME_ATTACK:
			m_state = State::StagePreview; return;

		case LOOP_CLEAR_STAGE:
		case LOOP_CLEAR_STAGE_TIME_ATTACK:
			m_state = State::ClearStage; return;

		case LOOP_RACE_RESULT: m_state = State::RaceResult; return;
		}

		m_state = State::None;
	}

	void Game::readSettings(const Json::Read& json)
	{
		m_settings.readSettings(json);
		m_input.readSettings(json);
	}

	void Game::writeSettings(Json::Write* json)
	{
		m_settings.writeSettings(json);
		m_input.writeSettings(json);
	}

	const Ram& Game::ram() const
	{
		return m_ram;
	}

	s32 Game::version() const
	{
		return m_version;
	}

	const Offset& Game::offset() const
	{
		return m_offset;
	}

	Settings* Game::settings()
	{
		return &m_settings;
	}

	InputWrapper* Game::input()
	{
		return &m_input;
	}

	s32 Game::state() const
	{
		return m_state;
	}
}
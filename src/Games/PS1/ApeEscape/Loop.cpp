#include "Loop.hpp"

#include "Common/PS1/Debug.hpp"
#include "Common/PS1/libgte.hpp"
#include "Common/PS1/PS1.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include <array>
#include <cmath>
#include <type_traits>

namespace PS1::ApeEscape
{
	enum
	{
		TITLE_SCREEN_NONE,
		TITLE_SCREEN_OPTION_MENU = 4,
		TITLE_SCREEN_MINI_GAME = 5,
		TITLE_SCREEN_SOUND_TEST = 6
	};

	using Fn_SetViewMatrix = std::array<Mips_t, 10>;

	static constexpr auto posMultiplyScalar{ 16.f };
	static constexpr auto fovMin{ 0.20f }, fovMax{ 4.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::NtscU)
		{
			d.loopIndexShift = 0x120;
			d.cShift = 0x59A8;
			d.csShift = 0x3B30;
			d.tpShift = 0x2CB0;
			d.ljShift2 = 0x2A38;
			d.bInstrShift = 0xFBF8;
			d.avShift = 0x200C;
			d.smShift = 0x3CF0;
			d.smShift2 = 0x9B30;
			d.gmShift = 0x2FD4;
			d.gmShift2 = 0x7AF0;
			d.gmInstr = 0x90C80000;
			d.tvInstr = 0x3C031B4E;
		}
		else if (version == Version::NtscJ)
		{
			d.loopIndexShift = 0x120;
			d.cShift = 0x5A18;
			d.csShift = 0x3BF0;
			d.tpShift = 0x2CB0;
			d.ljShift2 = 0x2A38;
			d.bInstrShift = 0xFBE8;
			d.avShift = 0x209C;
			d.smShift = 0x3CF4;
			d.smShift2 = 0x9B40;
			d.gmShift = 0x2FD4;
			d.gmShift2 = 0x7B20;
			d.gmInstr = 0x90C80000;
			d.tvInstr = 0x3C031B4E;
		}
		else
		{
			d.loopIndexShift = 0x128;
			d.cShift = 0x5A68;
			d.csShift = 0x3C40;
			d.tpShift = 0x2CF0;
			d.ljShift2 = 0x2A40;
			d.bInstrShift = 0xFC58;
			d.avShift = 0x20BC;
			d.smShift = 0x3D18;
			d.smShift2 = 0x9B70;
			d.gmShift = 0x2FDC;
			d.gmShift2 = 0x7B28;
			d.gmInstr = 0x90C30000;
			d.tvInstr = 0x27A50010;
		}

		if (version == Version::NtscU)
		{
			d.ssShift = 0x2560;
			d.tpShift2 = 0x2104;
			d.skrShift = 0x6348;
			d.sbShift = 0xBAF0;
			d.sbShift2 = 0xFD14;
			d.skrInstr = 0x0C04A306;
			d.skrInstr2 = 0x0C04A1EE;
			d.skrInstr3 = 0xA4432B40;
			d.sbInstr = 0x27840110;
			d.sbInstr2 = 0x878200EA;
			d.sbInstr3 = 0x978400D8;
			d.sbInstr4 = 0x0C047375;
			d.iglShift = 0x198;
			d.csShift2 = 0x1DC4;
			d.ssShift2 = 0xFA8;
			d.ssShift3 = 0x1280;
			d.spShift = 0x3248;
			d.spShift2 = 0x34D0;
			d.skrShift2 = 0x2EEC;
			d.skrShift3 = 0x228C8;
			d.sbShift3 = 0x39EC;
			d.sbShift4 = 0x3D7C;
			d.sbShift5 = 0xEA34;
			d.skrInstr4 = 0x0C049FA6;
			d.skrInstr5 = 0x0C0427E2;
			d.sbInstr5 = 0x0C047081;
			d.ljShift = 0x1FC04;
			d.bStructShift = 0x1F8;
			d.bShift = 0x135B8;
			d.ljXStructShift = 0x13C;
			d.cShift2 = 0x43C4;
			d.tpShift3 = 0x2490;
			d.tsShift = 0x1810;
			d.ssShift4 = 0x3BDC;
			d.skrShift4 = 0x42B40;
			d.sbShift6 = 0x35D38;
		}
		else
		{
			d.ssShift = 0x2524;
			d.tpShift2 = 0x2178;
			d.skrShift = 0x62A0;
			d.sbShift = 0xBA00;
			d.sbShift2 = 0xFC24;
			d.skrInstr = 0x0C04A2E1;
			d.skrInstr2 = 0x0C04A1C9;
			d.skrInstr3 = 0xA44327B8;
			d.sbInstr = 0x27840128;
			d.sbInstr2 = 0x87820102;
			d.sbInstr3 = 0x978400F0;
			d.sbInstr4 = 0x0C047339;
			d.iglShift = 0x164;
			d.csShift2 = 0x1E94;
			d.ssShift2 = 0xF74;
			d.ssShift3 = 0x1244;
			d.spShift = 0x3318;
			d.spShift2 = 0x35D8;
			d.skrShift2 = 0x2E44;
			d.skrShift3 = 0x22834;
			d.sbShift3 = 0x3B1C;
			d.sbShift4 = 0x3D5C;
			d.sbShift5 = 0xE944;
			d.skrInstr4 = 0x0C049F81;
			d.skrInstr5 = 0x0C0427B8;
			d.sbInstr5 = 0x0C047045;
			d.ljShift = 0x1FB5C;
			d.bStructShift = 0x228;
			d.bShift = 0x134C8;
			d.ljXStructShift = 0x154;
			d.cShift2 = 0x44CC;
			d.tpShift3 = 0x2504;
			d.tsShift = 0x1820;
			d.ssShift4 = 0x3B10;
			d.skrShift4 = 0x427B8;
			d.sbShift6 = 0x36208;
		}

		d.bStructShift2 = d.ljXStructShift + 0x38;
		d.ljYStructShift = d.ljXStructShift + 4;
		d.rjXStructShift = d.ljYStructShift + 4;
		d.rjYStructShift = d.rjXStructShift + 4;

		return d;
	}

	Loop::Loop(Ram&& ram, s32 version)
		: m_ram(std::move(ram)),
		m_version(version),
		m_offset(Offset::create(version)),
		m_input(&Game::baseInputs),
		m_controls(&m_input),
		m_dep(createVersionDependency(version))
	{
		const auto jsonRead{ Json::read(PS1::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			try
			{
				const auto& j{ jsonRead.value() };
				JSON_GET(j, m_resetZRotation);
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_hideHud);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableLJoystick);
				JSON_GET(j, m_disableRJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_noTvEffect);
				JSON_GET(j, m_titleScreenNoTimer);
				JSON_GET(j, m_stagePreviewNoTimer);
				m_input.readSettings(j);
			}
			catch (const Json::Exception& e)
			{
				Console::append(Console::Type::Exception, Json::exceptionFormat, Game::name, e.what());
			}
		}
	}

	Loop::~Loop()
	{
		Json::Write json;
		auto* const j{ &json };
		JSON_SET(j, m_resetZRotation);
		JSON_SET(j, m_pauseGame);
		JSON_SET(j, m_hideHud);
		JSON_SET(j, m_disableButton);
		JSON_SET(j, m_disableLJoystick);
		JSON_SET(j, m_disableRJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_noTvEffect);
		JSON_SET(j, m_titleScreenNoTimer);
		JSON_SET(j, m_stagePreviewNoTimer);
		m_input.writeSettings(&json);

		Json::overwrite(json, PS1::settingsFilePath(Game::name));

		if (isValid())
		{
			enable(false);
			update();
		}
	}

	void Loop::draw()
	{
		DRAW_GAME_WINDOWS(drawFreecam(), m_input.draw(), drawSettings(), drawBonus());
		PS1_DEBUG_DRAW_WINDOW;
	}

	void Loop::update()
	{
		const auto [vmo1, vmo2]{ Mips::li32(Mips::Register::a1, 0x80000000 + viewMatrixOffset()) };

		const Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0xAFBF0000, // sw ra,(sp)
			vmo1,
			vmo2,
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 32),
			0x8FBF0000, // lw ra,(sp)
			0x00000000, // nop
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		m_ram.write(m_offset.sonyLibrary, Fn_setViewMatrix);

		auto setState = [&]()
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
			else if (std::memcmp(bufferMgPtr, MINI2Begin.data(), bufferMg.size()) == 0)
			{
				m_state = State::SpecterBoxing;
				return;
			}
			else if (m_ram.read<u8>(m_offset.cutsceneState) == 1)
			{
				m_state = State::IngameCutscene;
				return;
			}

			const auto loopIndex{ m_ram.read<u16>(m_offset.globalStruct + m_dep.loopIndexShift) };
			
			if (loopIndex == LOOP_TRAINING_PREVIEW && m_ram.read<u16>(m_offset.fog) != 4096)
			{
				switch (m_ram.read<u16>(m_offset.globalStruct + m_dep.loopIndexShift + 6))
				{
				case 92:
				case 93:
				case 94:
				case 95:
				case 96:
				case 97:
				case 98:
					m_state = State::Ingame; return;
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
			case LOOP_GALAXY_MONKEY: m_state = State::GalaxyMonkey; return;
			}

			m_state = State::None;
		};

		setState();
		updateFreecam();
		updateBonus();
	}

	bool Loop::isValid()
	{
		const auto [offset, pattern]{ Game::offsetPattern(m_version) };
		return m_ram.isPatternValid(offset, pattern);
	}

	void Loop::drawFreecam()
	{
		Ui::setXSpacingStr("Movement Speed");
		FreecamModel::drawCameraName(Game::stateNames(), m_state);

		if (FreecamModel::drawEnable(&m_isEnabled))
		{
			enable(m_isEnabled);
		}

		FreecamModel::drawSeparatorProperties();
		drawCamera();
		FreecamModel::drawSeparatorOthers();
		drawOthers();
		FreecamModel::draw3DSettings();
	}

	void Loop::drawCamera()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed * posMultiplyScalar, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFov(&m_fov, !m_isEnabled, fovMin, fovMax);
	}

	void Loop::drawOthers()
	{
		if (m_state == State::Ingame)
		{
			MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
		}
		else
		{
			MiscModel::drawMiscHideHud(&m_isHudHidden, !m_isEnabled);
		}

		MiscModel::drawControllerButtonLRJoystick(&m_isButtonEnabled, &m_isLJoystickEnabled, &m_isRJoystickEnabled, !m_isEnabled);
	}

	void Loop::drawSettings()
	{
		Ui::setXSpacingStr("Disable Right Joystick");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Reset Z Rotation"), &m_resetZRotation);
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Hide Hud"), &m_hideHud);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Disable Left Joystick"), &m_disableLJoystick);
		Ui::checkbox(Ui::lol("Disable Right Joystick"), &m_disableRJoystick);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
	}

	void Loop::drawBonus()
	{
		static constexpr std::array<Ui::LabelSetter<s32>, 3> titleScreenChoices
		{
			"Option", TITLE_SCREEN_OPTION_MENU,
			"Mini Game", TITLE_SCREEN_MINI_GAME,
			"Sound Test", TITLE_SCREEN_SOUND_TEST
		};

		Ui::setXSpacingStr("Debug Menu");

		Ui::separatorText("Global");
		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Tv Effect"), &m_noTvEffect);

		Ui::separatorText("Title Screen");
		Ui::checkbox(Ui::lol("No Timer"), &m_titleScreenNoTimer);
		Ui::labelXSpacing("Debug Menu");
		Ui::buttonsSetter<s32>(titleScreenChoices, &m_titleScreenState);

		Ui::separatorText("Stage Select");
		MiscModel::drawEnableButton("Debug Menu", "Select Menu", &m_stageSelectSelectMenu);

		Ui::separatorText("Stage Preview");
		Ui::checkbox(Ui::lol("No Timer##2"), &m_stagePreviewNoTimer);
	}

	void Loop::updateFreecam()
	{
		updateOthers();
		updateCamera();

		if (m_input.isPressed(Input::ToggleFreecam))
		{
			enable(!m_isEnabled);
		}

		if (m_isEnabled)
		{
			FreecamModel::update3DSettings(&m_input,
				Input::MovementSpeedPos, Input::MovementSpeedNeg,
				Input::RotationSpeedPos, Input::RotationSpeedNeg,
				Input::FovSpeedPos, Input::FovSpeedNeg);

			const auto
				sx{ std::sin(m_rotation.x) },
				cx{ std::cos(m_rotation.x) },
				sy{ std::sin(m_rotation.y) },
				cy{ std::cos(m_rotation.y) },
				sz{ std::sin(m_rotation.z) },
				cz{ std::cos(m_rotation.z) },
				cc{ cy * cz },
				cs{ cy * sz },
				sc{ sy * cz },
				ss{ sy * sz };

			const auto
				fv{ m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward) * posMultiplyScalar },
				rv{ m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft) * posMultiplyScalar },
				uv{ m_controls.upVelocity(Input::MoveUp, Input::MoveDown) * posMultiplyScalar };

			m_positionFp.x += -cx * sy * fv;
			m_positionFp.y += sx * fv;
			m_positionFp.z += cx * cy * fv;
			m_positionFp.x += (cc - ss * sx) * rv;
			m_positionFp.y += -cx * sz * rv;
			m_positionFp.z += (sc + cs * sx) * rv;
			m_positionFp.x -= (cs + sc * sx) * uv;
			m_positionFp.y -= cz * cx * uv;
			m_positionFp.z -= (ss - cc * sx) * uv;

			CameraModel::rotatePitch(&m_rotation.x, m_controls.pitchVelocity(Input::RotateXPos, Input::RotateXNeg));
			CameraModel::rotateYaw(&m_rotation.y, m_controls.yawVelocity(Input::RotateYPos, Input::RotateYNeg));
			CameraModel::rotateRoll(&m_rotation.z, m_controls.rollVelocity(Input::RotateZPos, Input::RotateZNeg));
			CameraModel::increaseFov(&m_fov, m_controls.fovVelocity(Input::FovPos, Input::FovNeg), fovMin, fovMax);
		}
	}

	void Loop::updateCamera()
	{
		auto read = [&]()
		{
			if (m_state == State::SpaceMenu)
			{
				m_position = {};
				m_rotation = {};
				return;
			}

			libgte::MATRIX view;
			m_ram.read(vmPtr(m_state, true), &view);

			for (s32 i{}; i < 3; ++i)
			{
				auto* const vec{ (libgte::SVECTOR*)&view.m[i][0] };
				libgte::VectorNormalSS(vec, vec);
			}

			m_rotation.x = std::asin(fixedToFloat(view.m[2][1]));
			m_rotation.y = std::atan2(fixedToFloat(-view.m[2][0]), fixedToFloat(view.m[2][2]));
			m_rotation.z = std::atan2(fixedToFloat(-view.m[0][1]), fixedToFloat(view.m[1][1]));

			auto extractPosition = [&](bool minigame)
			{
				// Not 100% accurate but better than nothing
				auto* const v{ (s16*)&view.m };

				if (!minigame)
				{
					static constexpr auto yRatio{ floatToFixed(4096.f / 3040.f) };
					for (s32 i{}; i < 3; ++i)
					{
						v[3 + i] = (v[3 + i] * yRatio) >> 12;
					}
				}

				for (s32 i{}; i < 9; ++i)
				{
					v[i] = -v[i];
				}

				m_position = {};

				for (s32 i{}; i < 3; ++i)
				{
					m_position.x += (view.m[i][0] * view.t[i]) >> 12;
					m_position.y += (view.m[i][1] * view.t[i]) >> 12;
					m_position.z += (view.m[i][2] * view.t[i]) >> 12;
				}
			};

			switch (m_state)
			{
			case State::Ingame:
			case State::TitleScreen:
				m_ram.read(m_offset.cameraPosition, &m_position); break;
			case State::TrainingPreview:
				m_ram.read(m_offset.overlay + m_dep.tpShift, &m_position); break;
			case State::Cutscene:
			case State::StagePreview:
				m_ram.read(m_offset.overlay + m_dep.cShift, &m_position); break;
			case State::ClearStage:
				m_ram.read(m_offset.overlay + m_dep.csShift, &m_position); break;
			case State::AllVideo:
			case State::IngameCutscene:
				extractPosition(false); break;
			case State::RaceResult:
			case State::GalaxyMonkey:
				m_position = {}; break;
			case State::StageSelect:
				m_ram.read(0x001FFF90, &m_position); break;
			case State::SkiKidzRacing:
			case State::SpecterBoxing:
				extractPosition(true); break;
			}
		};

		auto write = [&]()
		{
			const auto decFp{ static_cast<Vec3<s16>>(m_positionFp) };
			m_positionFp -= decFp;
			m_position += decFp;

			const auto
				sx{ static_cast<s16>(floatToFixed(std::sin(m_rotation.x))) },
				cx{ static_cast<s16>(floatToFixed(std::cos(m_rotation.x))) },
				sy{ static_cast<s16>(floatToFixed(std::sin(m_rotation.y))) },
				cy{ static_cast<s16>(floatToFixed(std::cos(m_rotation.y))) },
				sz{ static_cast<s16>(floatToFixed(std::sin(m_rotation.z))) },
				cz{ static_cast<s16>(floatToFixed(std::cos(m_rotation.z))) },
				cc{ static_cast<s16>((cy * cz) >> 12) },
				cs{ static_cast<s16>((cy * sz) >> 12) },
				sc{ static_cast<s16>((sy * cz) >> 12) },
				ss{ static_cast<s16>((sy * sz) >> 12) };
		
			libgte::MATRIX view;
			view.m[0][0] = cc - ((ss * sx) >> 12);
			view.m[0][1] = (-cx * sz) >> 12;
			view.m[0][2] = sc + ((cs * sx) >> 12);
			view.m[1][0] = cs + ((sc * sx) >> 12);
			view.m[1][1] = (cz * cx) >> 12;
			view.m[1][2] = ss - ((cc * sx) >> 12);
			view.m[2][0] = (-cx * sy) >> 12;
			view.m[2][1] = sx;
			view.m[2][2] = (cx * cy) >> 12;
			view.t[0] = -m_position.x;
			view.t[1] = -m_position.y;
			view.t[2] = -m_position.z;

			auto* const trans{ (libgte::VECTOR*)&view.t };
			libgte::ApplyMatrixLV(&view, trans, trans);

			const auto 
				xFov{ static_cast<s16>(floatToFixed(m_fov)) },
				yFov{ static_cast<s16>(floatToFixed(m_fov * 0.7421875f)) };

			view.m[0][0] = view.m[0][0] * xFov >> 12;
			view.m[0][1] = view.m[0][1] * xFov >> 12;
			view.m[0][2] = view.m[0][2] * xFov >> 12;
			trans->vx = trans->vx * xFov >> 12;

			if (m_state == State::SkiKidzRacing || m_state == State::SpecterBoxing)
			{
				view.m[1][0] = view.m[1][0] * xFov >> 12;
				view.m[1][1] = view.m[1][1] * xFov >> 12;
				view.m[1][2] = view.m[1][2] * xFov >> 12;
				trans->vy = trans->vy * xFov >> 12;
			}
			else
			{
				view.m[1][0] = view.m[1][0] * yFov >> 12;
				view.m[1][1] = view.m[1][1] * yFov >> 12;
				view.m[1][2] = view.m[1][2] * yFov >> 12;
				trans->vy = trans->vy * yFov >> 12;
			}

			if (m_state == State::SkiKidzRacing)
			{
				auto* const v{ (s16*)&view.m };
				for (s32 i{}; i < 9; ++i)
				{
					v[i] *= 2;
				}
			}
			else
			{
				libgte::MATRIX billb{};
				billb.m[0][0] = cy;
				billb.m[0][2] = -sy;
				billb.m[1][1] = fixedOne;
				billb.m[2][0] = sy;
				billb.m[2][2] = cy;

				libgte::MulMatrix0(&view, &billb, &billb);
				m_ram.write(m_offset.billboardMatrix, billb.m);
				writeProjectionMatrix(xFov, yFov);
			}

			m_ram.write(vmPtr(m_state, false), view);
		};

		if (m_state != State::Ingame && m_state != State::TitleScreen && !m_isEnabled)
		{
			m_ram.write(m_offset.Fn_setViewMatrix, std::array<Mips_t, 2>{ 0x27BDFF90, 0xAFB50064 });
		}

		if (m_state == State::Ingame)
		{
			m_ram.write(m_offset.Fn_setViewMatrix, m_isEnabled ?
				std::array<Mips_t, 2>{ Mips::j(m_offset.sonyLibrary), 0x00C02021 } : std::array<Mips_t, 2>{ 0x27BDFF90, 0xAFB50064 });
		}
		else if (m_state == State::IngameCutscene || m_state == State::AllVideo)
		{
			m_ram.write(m_offset.overlay + m_dep.avShift, m_isEnabled ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFE8, 0xAFB00010 } );
		}
		else if (m_state == State::TitleScreen)
		{
			m_ram.write(m_offset.Fn_setViewMatrix, m_isEnabled ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFF90, 0xAFB50064 } );
		}
		else if (m_state == State::Cutscene || m_state == State::StagePreview || m_state == State::ClearStage || m_state == State::RaceResult)
		{
			m_ram.write(m_offset.overlay, m_isEnabled ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFF88, 0xAFB10064 } );
		}
		else if (m_state == State::StageSelect)
		{
			m_ram.writeConditional(m_isEnabled,
				m_offset.overlay + m_dep.ssShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB8, 0xAFB10034 },
				// Prevent breakpoints from div by zero when close to the "CLEARED" label
				m_offset.overlay + 0x970, 0x00000000, 0x0007000D,
				m_offset.overlay + 0x9A4, 0x00000000, 0x0007000D
			);
		}
		else if (m_state == State::SpaceMenu)
		{
			m_ram.write(m_offset.overlay + 0x8E8, m_isEnabled ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFB8, 0xAFB10034 } );
		}
		else if (m_state == State::TrainingPreview)
		{
			m_ram.writeConditional(m_isEnabled,
				m_offset.overlay, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF88, 0xAFB10064 },
				m_offset.overlay + m_dep.tpShift2, std::array<Mips_t, 2>{ 0x1000000D, 0x00000000 }, std::array<Mips_t, 2>{ 0xAD480000, 0xAD490004 }
			);
		}
		else if (m_state == State::SkiKidzRacing)
		{
			const auto offset{ m_offset.minigame + m_dep.skrShift };

			m_ram.writeConditional(m_isEnabled,
				offset,
					std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x10600005, 0x00000000 },
					std::array<Mips_t, 4>{ 0xAE000018, 0xAE000014, 0x10600005, 0xAE02001C },
				offset + 0x2C, 0x00000000, m_dep.skrInstr,
				offset + 0x2BC, 0x00000000, m_dep.skrInstr,
				offset + 0x4F0, 0x00000000, m_dep.skrInstr,
				offset + 0x1CD0, 0x00000000, m_dep.skrInstr,
				offset + 0x1C1C, 0x00000000, 0xAE400014,
				offset + 0x1C24, 0x00000000, 0xAE420018,
				offset + 0x1C34, 0x00000000, 0xAE42001C,
				offset + 0x1CE4, 0x00000000, m_dep.skrInstr2,
				offset + 0x1E24, 0x00000000, m_dep.skrInstr,
				offset + 0x1E30, 0x00000000, m_dep.skrInstr2,
				offset + 0x1F50, 0x00000000, m_dep.skrInstr,
				offset + 0x1F5C, 0x00000000, m_dep.skrInstr2,
				offset + 0x11CF8, 0x00000000, m_dep.skrInstr,
				offset + 0x11E28, 0x00000000, m_dep.skrInstr,
				offset + 0x12454, 0x1000000A, m_dep.skrInstr3,
				offset + 0x125CC, 0x240201C0, 0x2442FCC0,
				offset + 0x126EC, 0x240201C0, 0x2442FFC0,
				offset + 0x12CDC, 0x00000000, m_dep.skrInstr2,
				offset + 0x12D20, 0x00000000, 0xAE220014,
				offset + 0x12D54, 0x00000000, 0xAE220018,
				offset + 0x12D6C, 0x00000000, 0xAE22001C
			);
		}
		else if (m_state == State::SpecterBoxing)
		{
			const auto 
				offset{ m_offset.minigame + m_dep.sbShift },
				offset2{ m_offset.minigame + m_dep.sbShift2 };

			m_ram.writeConditional(m_isEnabled,
				offset, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, m_dep.sbInstr },
				offset + 0x144, Mips::jrRaNop(), std::array<Mips_t, 2>{ m_dep.sbInstr2, 0x27BDFFD8 },
				offset + 0x220, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, m_dep.sbInstr3 },
				offset2, 0x00000000, m_dep.sbInstr4,
				offset2 + 0x20, 0x00000000, 0xAD020018,
				offset2 + 0x2C, 0x00000000, 0xAD000014,
				offset2 + 0x34, 0x00000000, 0xAD02001C
			);
		}
		else if (m_state == State::GalaxyMonkey)
		{
			m_ram.writeConditional(m_isEnabled,
				m_offset.overlay + 0x77C, 0x00000000, Mips::jal(m_offset.Fn_initRotationMatrix),
				m_offset.overlay + 0x790, 0x00000000, Mips::jal(m_offset.Fn_CompMatrix),
				m_offset.overlay + 0xFFC, 0x00000000, Mips::jal(m_offset.Fn_initRotationMatrix),
				m_offset.overlay + 0x1014, 0x00000000, Mips::jal(m_offset.Fn_CompMatrix)
			);
		}

		m_isEnabled ? write() : read();
	}

	void Loop::updateOthers()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(&m_input, Input::HideHud, &m_isHudHidden);
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(&m_input, Input::LJoystick, &m_isLJoystickEnabled);
			MiscModel::toggle(&m_input, Input::RJoystick, &m_isRJoystickEnabled);

			if (m_state == State::Ingame)
			{
				MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
			}
			else
			{
				m_isGamePaused = false;
			}
		}

		const auto iglOffset{ m_offset.Fn_inGameLoop + m_dep.iglShift };

		m_ram.writeConditional(m_isGamePaused,
			iglOffset, 0x2402FFFF, 0x8E020100,
			iglOffset + 0x150, 0x2402FFFF, 0x8E020100,
			m_offset.Fn_updateAdditional + 0x30, 0x2402FFFF, 0x8C420100,
			m_offset.Fn_updateMonkey + 0x154, 0x2402FFFF, 0x8E020100,
			m_offset.Fn_updateCollectable + 0x18, 0x2402FFFF, 0x8C420100,
			m_offset.Fn_updateMovable + 0x34, 0x2402FFFF, 0x8C620100
		);

		m_ram.write(iglOffset + 0x34C, m_isHudHidden ? 0x00000000 : 0x10400007);

		if (m_version == Version::NtscJ || m_version == Version::NtscJRev1)
		{
			// Subtitles
			m_ram.write(m_offset.Fn_drawText, m_isHudHidden ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFF0, 0xAFB20008 });
		}

		if (m_state == State::TitleScreen)
		{
			m_ram.writeConditional(m_isHudHidden,
				m_offset.overlay + 0x1F8, 0x3C060400, 0x3C0600C8, // SCE
				m_offset.overlay + 0x26C, 0x34C60400, 0x34C60040, // Saru Get You Logo
				m_offset.overlay + 0xC6C, 0x1000001F, 0x1040001F // Cursor
			);
		}
		else if (m_state == State::StageSelect)
		{
			const auto 
				offset{ m_offset.overlay + m_dep.ssShift2 },
				offset2{ m_offset.overlay + m_dep.ssShift3 };

			m_ram.writeConditional(m_isHudHidden,
				offset, 0x24020400, 0x240200CA, // Buttons Text
				offset + 0x90, 0x24020400, 0x240200BC, // Buttons
				offset2, std::array<Mips_t,2>{ 0x240C0400, 0x240B0400 }, std::array<Mips_t,2>{ 0x240C00C0, 0x240B00C1 }, // LR Greenlines
				offset2 + 0x15E8, 0x24020400, 0x00A01021 // return str middle shift
			);
		}
		else if (m_state == State::SpaceMenu)
		{
			m_ram.writeConditional(m_isHudHidden,
				m_offset.overlay + m_dep.smShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF8, 0x3C07800F },
				m_offset.overlay + m_dep.smShift2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xAFB00010 }
			);
		}
		else if (m_state == State::StagePreview)
		{
			const auto offset{ m_offset.overlay + m_dep.spShift2 };

			m_ram.writeConditional(m_isHudHidden,
				m_offset.overlay + m_dep.spShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C02800F }, // Map info
				offset, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C03001E, 0x3C02800F }, // Stadium Attack
				offset + 0xC4, 0x3C050000, 0x3C050020 // WARNING! Y Size
			);
		}
		else if (m_state == State::ClearStage)
		{
			const auto offset{ m_offset.overlay + m_dep.csShift2 };

			m_ram.writeConditional(m_isHudHidden,
				m_offset.overlay + 0x12D0, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00804821, 0x3C02800F },
				offset, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x00003821 },
				offset + 0x5A8, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C02800F }
			);
		}
		else if (m_state == State::RaceResult)
		{
			m_ram.write(m_offset.overlay + 0x1D8, m_isHudHidden ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x18A00009, 0x00001821 } );
		}
		else if (m_state == State::SkiKidzRacing)
		{
			const auto offset{ m_offset.minigame + m_dep.skrShift2 };

			m_ram.writeConditional(m_isHudHidden,
				offset, 0x00000000, m_dep.skrInstr4,
				offset + 0x12C, 0x00000000, m_dep.skrInstr4,
				offset + 0x240, 0x00000000, m_dep.skrInstr4,
				offset + 0x6FC, 0x00000000, m_dep.skrInstr4,
				offset + 0xB68, 0x00000000, m_dep.skrInstr4,
				offset + 0x7414, 0x00000000, m_dep.skrInstr4,
				offset + 0xD9A4, 0x00000000, m_dep.skrInstr4 + 0x10, // Hide Char
				offset + 0x130A0, 0x00000000, m_dep.skrInstr4,
				m_offset.minigame + m_dep.skrShift3, 0x00000000, m_dep.skrInstr5
			);
		}
		else if (m_state == State::SpecterBoxing)
		{
			const auto 
				offset{ m_offset.minigame + m_dep.sbShift4 },
				offset2{ m_offset.minigame + m_dep.sbShift5 };

			m_ram.writeConditional(m_isHudHidden,
				m_offset.minigame + 0x1D1C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0xAFB20020 },
				m_offset.minigame + 0x1FE0, 0x00000000, m_dep.sbInstr5, 
				m_offset.minigame + 0x2428, 0x00000000, m_dep.sbInstr5,
				m_offset.minigame + m_dep.sbShift3, 0x00000000, m_dep.sbInstr5,
				offset, 0x00000000, m_dep.sbInstr5,
				offset + 0xC4, 0x00000000, m_dep.sbInstr5,
				offset + 0xE0, 0x00000000, m_dep.sbInstr5,
				offset + 0xFC, 0x00000000, m_dep.sbInstr5,
				offset + 0x118, 0x00000000, m_dep.sbInstr5,
				offset + 0x2CC, 0x00000000, m_dep.sbInstr5,
				offset2, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x38, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x74, 0x00000000, m_dep.sbInstr5,
				offset2 + 0xA8, 0x00000000, m_dep.sbInstr5,
				offset2 + 0xD0, 0x00000000, m_dep.sbInstr5,
				offset2 + 0xF8, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x120, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x148, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x1E8, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x290, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x338, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x3E0, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x6440, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x64AC, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x650C, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x6564, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x65C0, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x6650, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x66D0, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x6704, 0x00000000, m_dep.sbInstr5,
				offset2 + 0x673C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC8, 0xAFB00018 }
			);
		}
		else if (m_state == State::GalaxyMonkey)
		{
			const auto offset{ m_offset.overlay + m_dep.gmShift };

			m_ram.writeConditional(m_isHudHidden,
				m_offset.overlay + 0x1048, 0x00001021, 0x24020100, // Logo Y Size
				m_offset.overlay + 0x2368, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x00805021 }, // Text
				m_offset.overlay + 0x24D4, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x8C4A0004, m_dep.gmInstr }, // Mid Text
				offset, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C085555, 0x35085555 }, // Green Frame
				offset + 0x2470, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C07E100, 0x34E7020F }, // Life
				offset + 0x3AC0, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00005821, 0x3C028014 }, // Large Text
				offset + 0x421C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC8, 0x3C04800F },
				m_offset.overlay + m_dep.gmShift2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xAFB00010 } // Text
			);
		}

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn_padStatus + 0x1B8, 0xAE240158, 0xAE200158,
			m_offset.Fn_padStatus + 0x1C0, 0xAE220160, 0xAE200160
		);

		m_ram.write(m_offset.Fn_padStatus + 0x464, m_isLJoystickEnabled ? 0xA6420180 : 0xA6400180);
		
		m_ram.writeConditional(m_isRJoystickEnabled,
			m_offset.Fn_padStatus + 0x3DC, 0xA6420182, 0xA6400182,
			m_offset.Fn_padStatus + 0x3F0, 0xA6420182, 0xA6400182
		);

		if (m_state == State::SkiKidzRacing)
		{
			const auto offset{ m_offset.minigame + m_dep.ljShift };

			m_ram.write(offset + 0xFC, m_isButtonEnabled ? 0xAF820000 + m_dep.bStructShift : 0xAF800000 + m_dep.bStructShift);

			m_ram.writeConditional(m_isLJoystickEnabled,
				offset, 0xA4430000, 0xA4400000,
				offset + 0xC, 0xA4430000, 0xA4400000
			);

			m_ram.writeConditional(m_isRJoystickEnabled,
				offset + 0x30, 0xA4430000, 0xA4400000,
				offset + 0x40, 0xA4430000, 0xA4400000
			);
		}
		else if (m_state == State::SpecterBoxing)
		{
			const auto offset{ m_offset.minigame + m_dep.bShift };

			m_ram.write(offset, m_isButtonEnabled ? 0xAF820000 + m_dep.bStructShift2 : 0xAF800000 + m_dep.bStructShift2);
			m_ram.write(offset + 0x1104, m_isLJoystickEnabled ? 
				std::array<Mips_t, 2>{ 0xA7830000 + m_dep.ljXStructShift, 0xA7820000 + m_dep.ljYStructShift } : 
				std::array<Mips_t, 2>{ 0xA7800000 + m_dep.ljXStructShift, 0xA7800000 + m_dep.ljYStructShift }
			);

			m_ram.write(offset + 0x1128, m_isRJoystickEnabled ? 
				std::array<Mips_t, 2>{ 0xA7830000 + m_dep.rjXStructShift, 0xA7820000 + m_dep.rjYStructShift } : 
				std::array<Mips_t, 2>{ 0xA7800000 + m_dep.rjXStructShift, 0xA7800000 + m_dep.rjYStructShift }
			);
		}
		else if (m_state == State::GalaxyMonkey)
		{
			const auto offset{ m_offset.overlay + m_dep.ljShift2 };

			m_ram.write(m_offset.overlay + 0x1B4, m_isButtonEnabled ? 0xAC440000 + m_dep.bInstrShift : 0xAC400000 + m_dep.bInstrShift);

			m_ram.writeConditional(m_isLJoystickEnabled,
				offset, 0xA2020010, 0xA2000010,
				offset + 0x10, 0xA2020011, 0xA2000011
			);

			m_ram.writeConditional(m_isRJoystickEnabled,
				offset + 0x20, 0xA2020012, 0xA2000012,
				offset + 0x34, 0xA2020013, 0xA2000013
			);
		}
	}

	void Loop::updateBonus()
	{
		m_ram.writeConditional(m_noFog,
			m_offset.Fn_drawActive + 0x90, 0x24047FFF, 0x94440028,
			m_offset.Fn_drawActive + 0xEC, 0x24057FFF, 0x94450028,
			m_offset.Fn_drawLayer + 0x68, 0x24097FFF, 0x8E49002C,
			m_offset.Fn_drawLayer + 0xB4, 0x24087FFF, 0x96480028,
			m_offset.Fn_drawLayer2 + 0x124, 0x24057FFF, 0x94E50028,
			m_offset.Fn_drawLayer2 + 0x12C, 0x24067FFF, 0x94460016,
			m_offset.Fn_drawEnemy + 0x12C, 0x24047FFF, 0x94440028
		);

		if (m_state == State::TitleScreen)
		{
			m_ram.write(m_offset.overlay + 0x1DC, m_titleScreenNoTimer ? 0x10000005 : 0x14400005);
		}
		else if (m_state == State::Cutscene)
		{
			m_ram.write(m_offset.overlay + m_dep.cShift2, m_noTvEffect ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFB8, m_dep.tvInstr });
		}
		else if (m_state == State::TrainingPreview)
		{
			m_ram.write(m_offset.overlay + m_dep.tpShift3, m_noTvEffect ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFB8, m_dep.tvInstr });
		}
		else if (m_state == State::StagePreview)
		{
			m_ram.write(m_offset.overlay + 0x1F20, m_stagePreviewNoTimer ? 0x1000005D : 0x1462005D);
		}

		if (m_titleScreenState != TITLE_SCREEN_NONE)
		{
			if (m_state == State::TitleScreen)
			{
				m_ram.write(m_offset.overlay + m_dep.tsShift, m_titleScreenState);
			}
			else
			{
				Console::append(Console::Type::Error, "Not in title screen");
			}
			m_titleScreenState = TITLE_SCREEN_NONE;
		}

		if (m_stageSelectSelectMenu)
		{
			if (m_state == State::StageSelect)
			{
				m_ram.write(m_offset.overlay + m_dep.ssShift4, 0);
			}
			else
			{
				Console::append(Console::Type::Error, "Not in stage select");
			}
			m_stageSelectSelectMenu = false;
		}
	}

	void Loop::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			FreecamModel::resetSpeed(m_resetMovementSpeed, m_resetRotationSpeed, m_resetFovSpeed);
			m_controls.resetVelocity();

			if (m_resetZRotation)
			{
				m_rotation.z = 0.f;
			}
			if (m_pauseGame)
			{
				m_isGamePaused = true;
			}
			if (m_hideHud)
			{
				m_isHudHidden = true;
			}
			if (m_disableButton)
			{
				m_isButtonEnabled = false;
			}
			if (m_disableLJoystick)
			{
				m_isLJoystickEnabled = false;
			}
			if (m_disableRJoystick)
			{
				m_isRJoystickEnabled = false;
			}
		}
		else
		{
			m_isGamePaused = false;
			m_isHudHidden = false;
			m_isButtonEnabled = true;
			m_isLJoystickEnabled = true;
			m_isRJoystickEnabled = true;
			m_fov = 1.f;

			writeProjectionMatrix(4096, 3040);

			if (m_state == State::GalaxyMonkey)
			{
				libgte::MATRIX vm{};
				vm.m[0][0] = 4096;
				vm.m[1][1] = 3040;
				vm.m[2][2] = 4096;
				m_ram.write(m_offset.viewMatrix, vm);
			}
		}
	}

	u32 Loop::vmPtr(s32 state, bool read) const
	{
		switch (state)
		{
		case State::Ingame: return read ? m_offset.viewMatrix : viewMatrixOffset();
		case State::SkiKidzRacing: return m_offset.minigame + m_dep.skrShift4;
		case State::SpecterBoxing: return m_offset.minigame + m_dep.sbShift6;
		default: return m_offset.viewMatrix;
		}
	}

	void Loop::writeProjectionMatrix(s16 x, s16 y) const
	{
		libgte::MATRIX p{};
		p.m[0][0] = x;
		p.m[1][1] = y;
		p.m[2][2] = 4096;
		m_ram.write(m_offset.projectionMatrix, p.m);
	}

	u32 Loop::viewMatrixOffset() const
	{
		return m_offset.sonyLibrary + sizeof(Fn_SetViewMatrix);
	}
}
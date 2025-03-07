#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/Math.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include <array>
#include <cmath>
#include <type_traits>

namespace PS2::DragonQuest8
{
	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.nearShift = 0x20;
			d.ubShift = 0xF0;
			d.cgsShift = 0x3864;
			d.spmInstr = 0xE4800014;
			d.spmInstr2 = 0xE48C0014;
			d.tInstr = 0xC60C385C;
			d.dchInstr = 0x10000073;
			d.dchInstr2 = 0x12A00073;
			d.dbhInstr = 0x100000EF;
			d.dbhInstr2 = 0x104000EF;
			d.tsInstr = 0xE4816B10;
		}
		else
		{
			d.nearShift = 0x1C;
			d.ubShift = 0xE0;
			d.cgsShift = 0x3860;
			d.spmInstr = 0xE4800010;
			d.spmInstr2 = 0xE48C0010;
			d.tInstr = 0x3C023F80;
			d.dchInstr = 0x10000069;
			d.dchInstr2 = 0x12600069;
			d.dbhInstr = 0x100000EB;
			d.dbhInstr2 = 0x104000EB;
			d.tsInstr = 0xE4816B00;
		}

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
		const auto jsonRead{ Json::read(PS2::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			try
			{
				const auto& j{ jsonRead.value() };
				JSON_GET(j, m_resetZRotation);
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_hideHud);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noNearClipTransparency);
				JSON_GET(j, m_timer.isFrozen);
				if (m_timer.isFrozen)
				{
					JSON_GET_MIN_MAX(j, m_timer.time, 0.f, 24.f);
				}
				JSON_GET(j, m_noRandomEncounter);
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
		JSON_SET(j, m_disableJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noNearClipTransparency);
		JSON_SET(j, m_timer.time);
		JSON_SET(j, m_timer.isFrozen);
		JSON_SET(j, m_noRandomEncounter);
		m_input.writeSettings(&json);

		Json::overwrite(json, PS2::settingsFilePath(Game::name));

		if (isValid())
		{
			enable(false);
			update();
		}
	}

	void Loop::draw()
	{
		DRAW_GAME_WINDOWS(drawFreecam(), m_input.draw(), drawSettings(), drawBonus());
		PS2_PCSX2_DRAW_PNACH_BEHAVIOR_WINDOW(Game);
		PS2_DEBUG_DRAW_WINDOW(Game);
	}

	void Loop::update()
	{
		const auto gameState{ m_ram.read<s32>(m_offset.gameState) };

		switch (gameState)
		{
		case 0: m_state = State::Common; break;
		case 1: m_state = State::Cutscene; break;
		case 3: m_state = State::Companions; break;
		case 6: m_state = State::Alchemy_Minimap; break;
		case 7: m_state = State::Battle; break;
		default: m_state = State::None;
		}

		m_ram.read(m_offset.cameraPtr, &m_cameraPtr);

		if (m_cameraPtr)
		{
			m_ram.read(m_cameraPtr + (((gameState << 4) - gameState) << 2) + m_dep.cgsShift + 0x38, &m_cameraPtr);
		}

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
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Loop::drawOthers()
	{
		MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
		MiscModel::drawControllerButtonJoystick(&m_isButtonEnabled, &m_isJoystickEnabled, !m_isEnabled);
	}

	void Loop::drawSettings()
	{
		Ui::setXSpacingStr("Reset Movement Speed");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Reset Z Rotation"), &m_resetZRotation);
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Hide Hud"), &m_hideHud);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Disable Joystick"), &m_disableJoystick);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
	}

	void Loop::drawBonus()
	{
		Ui::setXSpacingStr("No Near Clip Transparency");

		Ui::checkbox(Ui::lol("No Near Clip Transparency"), &m_noNearClipTransparency);
		Ui::hoveredTooltip("Prevents entities from disappearing on the screen when the camera is too close");

		const auto timeInt{ static_cast<u32>(m_timer.time) };
		auto [hour, minute]{ std::array<u32, 2>{ timeInt, static_cast<u32>((m_timer.time - timeInt) * 60) } };

		Ui::labelXSpacing("Time");
		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
		m_timer.shouldChangeValue |= Ui::dragWrap("##Hour", &hour, 1u, "%dh", 0, 0u, 23u);
		ImGui::SameLine();
		m_timer.shouldChangeValue |= Ui::dragWrap("##Time", &minute, 1u, "%dm", 0, 0u, 59u);
		ImGui::PopItemWidth();
		if (m_timer.shouldChangeValue)
		{
			m_timer.time = static_cast<float>(hour) + static_cast<float>(minute / 59.99f);
		}
		ImGui::SameLine();
		Ui::checkbox("Frozen", &m_timer.isFrozen);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("No Random Encounter"), &m_noRandomEncounter);
		MiscModel::drawEnableButton("Bag items", "Unlock all", &m_giftItems);
		Ui::hoveredTooltip("Gift all items in the bag in 999 copies");
		MiscModel::drawEnableButton("Stats", "Set max", &m_statsMax);
		Ui::hoveredTooltip("Set stats at max for Hero, Yangus, Jessica and Angelo");
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
				sx{ -std::sin(m_rotation.x) },
				cx{ std::cos(m_rotation.x) },
				sy{ -std::sin(m_rotation.y) },
				cy{ std::cos(m_rotation.y) },
				sz{ -std::sin(m_rotation.z) },
				cz{ std::cos(m_rotation.z) },
				cc{ cz * cy },
				cs{ cz * sy },
				sc{ sz * cy },
				ss{ sz * sy };

			const auto
				fv{ m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward) },
				rv{ m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft) },
				uv{ m_controls.upVelocity(Input::MoveUp, Input::MoveDown) };

			m_position.x -= -cx * sy * fv;
			m_position.y -= sx * fv;
			m_position.z -= cx * cy * fv;
			m_position.x += (cc - ss * sx) * rv;
			m_position.y += -cx * sz * rv;
			m_position.z += (cs + sc) * rv;
			m_position.x += (sc + cs * sx) * uv;
			m_position.y += cz * cx * uv;
			m_position.z += (ss - cc * sx) * uv;

			CameraModel::rotatePitch(&m_rotation.x, m_fov, m_controls.pitchVelocity(Input::RotateXPos, Input::RotateXNeg));
			CameraModel::rotateYaw(&m_rotation.y, m_fov, m_controls.yawVelocity(Input::RotateYPos, Input::RotateYNeg));
			CameraModel::rotateRoll(&m_rotation.z, m_controls.rollVelocity(Input::RotateZPos, Input::RotateZNeg));
			CameraModel::increaseFov(&m_fov, m_controls.fovVelocity(Input::FovPos, Input::FovNeg));
		}
	}

	void Loop::updateCamera()
	{
		auto read = [&]()
		{
			// For some reasons, position and rotation of the minimap are completely broken
			if (m_state == State::Alchemy_Minimap && !PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.alchemyPtr)))
			{
				m_position = {};
				m_rotation = {};
				return;
			}

			float rm[3][4];
			m_ram.read(m_offset.fov, &m_fov);
			m_ram.read(m_cameraPtr + 0x40, &m_position);
			m_ram.read(m_cameraPtr + 0x10, &rm);
			m_rotation.x = -std::asin(rm[2][1]);
			m_rotation.y = -std::atan2(-rm[2][0], rm[2][2]);
			m_rotation.z = -std::atan2(-rm[0][1], rm[1][1]);
		};

		auto write = [&]()
		{
			const auto
				sx{ -std::sin(m_rotation.x) },
				cx{ std::cos(m_rotation.x) },
				sy{ -std::sin(m_rotation.y) },
				cy{ std::cos(m_rotation.y) },
				sz{ -std::sin(m_rotation.z) },
				cz{ std::cos(m_rotation.z) },
				cc{ cz * cy },
				cs{ cz * sy },
				sc{ sz * cy },
				ss{ sz * sy };

			float rm[3][3];
			rm[0][0] = cc - ss * sx;
			rm[0][1] = -cx * sz;
			rm[0][2] = cs + sc * sx;
			rm[1][0] = sc + cs * sx;
			rm[1][1] = cz * cx;
			rm[1][2] = ss - cc * sx;
			rm[2][0] = -cx * sy;
			rm[2][1] = sx;
			rm[2][2] = cx * cy;

			const bool isInCutscene{ m_ram.read<s32>(m_offset.cutsceneState) == 2 };

			if (m_state == State::Common && !isInCutscene)
			{
				auto eye{ m_position };
				static constexpr auto z{ 9.999996185f };
				eye.x += -rm[2][0] * z;
				eye.y += -rm[2][1] * z;
				eye.z += -rm[2][2] * z;

				const std::array<float, 23> packet
				{
					rm[0][0], rm[0][1], rm[0][2], 0.f,
					rm[1][0], rm[1][1], rm[1][2], 0.f,
					rm[2][0], rm[2][1], rm[2][2], 0.f,
					m_position.x, m_position.y, m_position.z, 1.f,
					eye.x, eye.y, eye.z, 1.f,
					m_position.x, m_position.y, m_position.z
				};

				m_ram.write(m_cameraPtr + 0x10, packet);
			}
			else
			{
				const std::array<float, 15> packet
				{
					rm[0][0], rm[0][1], rm[0][2], 0.f,
					rm[1][0], rm[1][1], rm[1][2], 0.f,
					rm[2][0], rm[2][1], rm[2][2], 0.f,
					m_position.x, m_position.y, m_position.z
				};

				m_ram.write(m_cameraPtr + 0x10, packet);
			}

			m_ram.write(m_offset.fov, m_fov);
			m_ram.write(m_offset.projectionStruct + m_dep.nearShift, 1.f);
		};

		if (m_version != Version::Pal)
		{
			// Workaround to fix a skybox bug when the pitch is high or low, fixed in pal version
			m_ram.write(m_offset.Fn_drawSkybox + 0x8C, std::fabs(m_rotation.x) > 0.6981316f ? 0x10000096 : 0x45000096);
		}

		const auto jalAddVector{ Mips::jal(m_offset.Fn_mgAddVector__FPfPf) };

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_Step__14CCameraControlFf + 0x94, 0x00000000, jalAddVector,
			m_offset.Fn_Step__14CCameraControlFf + 0xD8, 0x00000000, jalAddVector,
			m_offset.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo + 0x1C, 0x00000000, m_dep.spmInstr, // Fov
			m_offset.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo + 0x1C0, 0x00000000, m_dep.spmInstr2, // Fov
			m_offset.Fn_Roll__9mgCCameraFf, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF50, 0xFFBF0020 },
			m_offset.Fn_SetPos__7CCameraFPf + 0x10, 0x00000000, 0x7C830060,
			m_offset.Fn_SetPos__7CCameraFPf + 0x30, 0x00000000, 0x7C830050, // Cutscene
			m_offset.Fn_drawCompass, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFC10, 0xFFBF0090 }
		);

		if (m_cameraPtr)
		{
			const auto writePositionCallback{ m_ram.read<u32>(m_cameraPtr + 0xC0) + 0x40 };
			if (writePositionCallback > Game::pnachInfo(m_version).textSectionEnd)
			{
				m_ram.write(writePositionCallback, m_isEnabled ? m_offset.Fn_nullsub1 : m_offset.Fn_CreatePose__7CCameraFv);
			}

			m_isEnabled ? write() : read();
		}
	}

	void Loop::updateOthers()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
			MiscModel::toggle(&m_input, Input::HideHud, &m_isHudHidden);
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(&m_input, Input::Joystick, &m_isJoystickEnabled);
		}

		m_ram.writeConditional(m_isGamePaused,
			m_offset.Fn_npcStep + 0xB4, 0x44806000, m_dep.tInstr,
			m_offset.Fn_playerStep + 0x88, 0x44806000, m_dep.tInstr,
			m_offset.walkSpeed, 0.f, 3.8f,
			m_offset.Fn_updateBattle + m_dep.ubShift, 0x00000000, 0x50400003, // Next step anim
			m_offset.Fn_npcScript + 0x10, 0x1000000A, 0x1060000A
		);

		m_ram.writeConditional(m_isHudHidden,
			m_offset.Fn_drawCommonHud + 0x28, m_dep.dchInstr, m_dep.dchInstr2,
			m_offset.Fn_drawBattleHud + 0x3C, m_dep.dbhInstr, m_dep.dbhInstr2,
			m_offset.Fn_drawMidTextOnScreen + 0x98, 0x00000000, Mips::jal(m_offset.Fn_drawTextOnScreen)
		);

		if (m_version == Version::Pal)
		{
			m_ram.write(m_offset.Fn_padStatus + 0xDC, m_isButtonEnabled ? 0x3263FFFF : 0x00001821);
			m_ram.writeConditional(m_isJoystickEnabled,
				m_offset.Fn_padStatus + 0x108, 0x93A20092, 0x2402007F,
				m_offset.Fn_padStatus + 0x110, 0x93A20093, 0x2402007F,
				m_offset.Fn_padStatus + 0x118, 0x93A20094, 0x2402007F,
				m_offset.Fn_padStatus + 0x120, 0x93A20095, 0x2402007F
			);
		}
		else
		{
			m_ram.write(m_offset.Fn_padStatus + 0x70, m_isButtonEnabled ? 0x3043FFFF : 0x00001821);
			m_ram.writeConditional(m_isJoystickEnabled,
				m_offset.Fn_padStatus + 0x80, 0x93A20034, 0x2402007F,
				m_offset.Fn_padStatus + 0x88, 0x93A20035, 0x2402007F,
				m_offset.Fn_padStatus + 0x90, 0x93A20036, 0x2402007F,
				m_offset.Fn_padStatus + 0x98, 0x93A20037, 0x2402007F
			);
		}
	}

	void Loop::updateBonus()
	{
		m_ram.write(m_offset.Fn_randomEncounterBehavior + 0x18C, m_noRandomEncounter ? 0x00000000 : 0xE6410024);
		m_ram.write(m_offset.Fn_TimeStep__8dqCSceneFf + 0xBC, m_timer.isFrozen ? 0x00000000 : m_dep.tsInstr);
		m_ram.writeConditional(m_noNearClipTransparency,
			m_offset.Fn_DrawStep__12CModelObjectFf + 0x5C, 0x10000006, 0x54600006,
			m_offset.Fn_drawMainCharacter + 0x3BC, 0x00000000, 0x10400003
		);

		if (m_timer.shouldChangeValue || m_timer.isFrozen)
		{
			m_ram.write(m_offset.timer, m_timer.time);
			m_timer.shouldChangeValue = false;
		}
		else if (!m_timer.isFrozen)
		{
			m_ram.read(m_offset.timer, &m_timer.time);
		}

		if (m_giftItems)
		{
			constexpr auto items999 = []()
			{
				std::array<u32, 388> items;
				for (std::size_t i{}; i < items.size(); ++i)
				{
					items[i] = (static_cast<u32>(i) + 1) + (999 << 16);
				}
				return items;
			};

			m_ram.write(m_offset.bag, items999());
			Console::append(Console::Type::Success, "Generated items successfully");
			m_giftItems = false;
		}

		if (m_statsMax)
		{
			auto currentOffset{ m_offset.heroStats };

			// Hero, Yangus, Jessica, Angelo
			for (s32 i{}; i < 4; ++i)
			{
				// Max hp, Hp, Max mp, Mp
				m_ram.write(currentOffset, std::array<s32, 4>{ 999, 999, 999, 999 });

				// Level
				currentOffset += 0x10;
				m_ram.write(currentOffset, s16(98));

				// Spells
				currentOffset += 0x18;
				m_ram.write(currentOffset, -1);

				// Str, agi, res, wis
				currentOffset += 8;
				m_ram.write(currentOffset, std::array<s16, 4>{ 999, 999, 999, 999 });

				// Skills
				currentOffset += 0xA;
				m_ram.write(currentOffset, std::array<s16, 5>{ 100, 100, 100, 100, 100 });

				// Next character
				currentOffset += 0x2E;
			}

			Console::append(Console::Type::Success, "Set stats max successfully");
			m_statsMax = false;
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
			if (m_disableJoystick)
			{
				m_isJoystickEnabled = false;
			}
		}
		else
		{
			m_isGamePaused = false;
			m_isHudHidden = false;
			m_isButtonEnabled = true;
			m_isJoystickEnabled = true;

			if (m_cameraPtr)
			{
				m_ram.write(m_offset.fov, Math::toRadians(m_ram.read<float>(m_cameraPtr)));
				m_ram.write(m_offset.projectionStruct + m_dep.nearShift, 5.f);
			}
		}
	}
}
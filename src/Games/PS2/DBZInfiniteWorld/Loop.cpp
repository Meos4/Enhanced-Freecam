#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include <array>
#include <cmath>
#include <type_traits>

namespace PS2::DBZInfiniteWorld
{
	static constexpr auto duPosMultiplyScalar{ 32.f };
	static constexpr auto timescaleMin{ 0.1f }, timescaleMax{ 3.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.timescaleBase = 1.2f;
		}
		else
		{
			d.timescaleBase = 1.f;
		}

		if (version == Version::NtscU)
		{
			d.m014ubInstr = 0x27BDFF00;
			d.m019uInstr = 0x27BDFF90;
		}
		else
		{
			d.m014ubInstr = 0x27BDFEF0;
			d.m019uInstr = 0x27BDFFA0;
		}

		return d;
	}

	Loop::Loop(Ram&& ram, s32 version)
		: m_ram(std::move(ram)),
		m_version(version),
		m_offset(Offset::create(version)),
		m_input(&Game::baseInputs),
		m_controls(&m_input),
		m_dep(createVersionDependency(version)),
		m_timescale(m_dep.timescaleBase)
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
				JSON_GET(j, m_displayMode);
				JSON_GET(j, m_noAuras);
				JSON_GET(j, m_noCelShading);
				JSON_GET_MIN_MAX(j, m_shadersMode, 0, SHADERS_COUNT - 1);
				JSON_GET(j, m_noShadersHighlight);
				JSON_GET(j, m_noOnScreenEffects);
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
		JSON_SET(j, m_displayMode);
		JSON_SET(j, m_noAuras);
		JSON_SET(j, m_noCelShading);
		JSON_SET(j, m_shadersMode);
		JSON_SET(j, m_noShadersHighlight);
		JSON_SET(j, m_noOnScreenEffects);
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
		const auto battleCameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };

		if (PS2::isValidMemoryRange(battleCameraPtr))
		{
			if (PS2::isValidMemoryRange(m_ram.read<u32>(battleCameraPtr + 0x240)))
			{
				m_state = State::Battle;
			}
			else if (PS2::isValidMemoryRange(m_ram.read<u32>(battleCameraPtr + 0x244)))
			{
				m_state = State::DragonMission;
			}
			else
			{
				m_state = State::None;
			}
		}
		else if (const auto flightCameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr) };
			PS2::isValidMemoryRange(flightCameraPtr))
		{
			if (PS2::isValidMemoryRange(flightCameraPtr) && m_ram.read<float>(flightCameraPtr + 0x64C) == 1.f)
			{
				m_state = State::DragonMissionFlying;
			}
			else if (PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.FLIGHT_cameraPtr + 0x70)))
			{
				m_state = State::DragonMissionCutscene;
			}
			else
			{
				m_state = State::None;
			}
		}
		else
		{
			m_state = State::None;
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
		auto posSpeed{ g_settings.dragFloatSpeed };

		if (m_state == State::DragonMissionFlying)
		{
			posSpeed *= duPosMultiplyScalar;
		}

		CameraModel::drawPosition(&m_position, posSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Loop::drawOthers()
	{
		if (m_state != State::DragonMissionCutscene && m_state != State::DragonMissionFlying)
		{
			MiscModel::drawTimescale(&m_timescale, timescaleMin * m_dep.timescaleBase, timescaleMax * m_dep.timescaleBase, !m_isEnabled);
		}

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
		Ui::setXSpacingStr("No On Screen Effects");

		Ui::separatorText("Global");
		Ui::checkbox(Ui::lol("16:9"), &m_displayMode);
		Ui::hoveredTooltip("Turn the game in 16:9 aspect ratio, this mode is official but unused / unfinished");
		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Cel Shading"), &m_noCelShading);
		Ui::checkbox(Ui::lol("No Shaders Highlight"), &m_noShadersHighlight);
		ImGui::Combo(Ui::lol("Shaders"), &m_shadersMode, "Normal\0No Shaders\0No Textures\0");

		Ui::separatorText("Battle");
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/grey lines");

		Ui::separatorText("Cheats");
		MiscModel::drawEnableButton("Unlock All", "Set", &m_unlockAll);
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

			auto
				fv{ m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward) },
				rv{ m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft) },
				uv{ m_controls.upVelocity(Input::MoveUp, Input::MoveDown) };

			if (m_state == State::DragonMissionFlying)
			{
				fv *= duPosMultiplyScalar;
				rv *= duPosMultiplyScalar;
				uv *= duPosMultiplyScalar;
			}

			m_position.x -= cx * sy * fv;
			m_position.y -= -sx * fv;
			m_position.z -= cy * cx * fv;
			m_position.x += (cc + ss * sx) * rv;
			m_position.y += cx * sz * rv;
			m_position.z += (cs * sx - sc) * rv;
			m_position.x += (sc * sx - cs) * uv;
			m_position.y += cx * cz * uv;
			m_position.z += (cc * sx + ss) * uv;

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
			float vm[3][4];

			if (m_state == State::Battle || m_state == State::DragonMission)
			{
				const auto cameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };
				const auto cameraPtrState{ m_ram.read<u32>(cameraPtr + 0x244) };

				m_ram.read(cameraPtrState, &m_position);
				m_ram.read(cameraPtrState + 0x34, &m_fov);
				m_ram.read(cameraPtr + 0x70, &vm);
			}
			else if (m_state == State::DragonMissionFlying)
			{
				const auto cameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr) };

				m_ram.read(cameraPtr + 0x640, &m_position);
				m_ram.read(cameraPtr + 0x650, &vm);
				m_ram.read(cameraPtr + 0x6CC, &m_fov);
				m_fov = 448.f / m_fov / 2.f;
			}
			else
			{
				const auto cameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr + 0x70) };

				m_ram.read(cameraPtr + 0x660, &m_position);
				m_ram.read(cameraPtr + 0x694, &m_fov);
				m_ram.read(cameraPtr + 0x590, &vm);
			}

			m_rotation.x = std::asin(-vm[1][2]);
			m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
			m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
		};

		auto write = [&]()
		{
			if (m_state == State::DragonMissionFlying)
			{
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

				const auto px{ -m_position.x }, py{ -m_position.y }, pz{ -m_position.z };

				float vm[4][4];
				vm[0][0] = cc + ss * sx;
				vm[0][1] = sc * sx - cs;
				vm[0][2] = cx * sy;
				vm[1][0] = cx * sz;
				vm[1][1] = cx * cz;
				vm[1][2] = -sx;
				vm[2][0] = cs * sx - sc;
				vm[2][1] = cc * sx + ss;
				vm[2][2] = cy * cx;
				vm[3][0] = vm[0][0] * px + vm[1][0] * py + vm[2][0] * pz;
				vm[3][1] = vm[0][1] * px + vm[1][1] * py + vm[2][1] * pz;
				vm[3][2] = vm[0][2] * px + vm[1][2] * py + vm[2][2] * pz;
				vm[3][3] = 1.f;
				vm[0][3] = vm[1][3] = vm[2][3] = 0.f;

				const auto cameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr) };
				m_ram.write(cameraPtr + 0x650, vm);
				m_ram.write(cameraPtr + 0x6CC, 448.f / (m_fov * 2.f));
			}
			else
			{
				const auto
					dx{ m_rotation.x * 0.5f },
					dy{ m_rotation.y * 0.5f },
					dz{ m_rotation.z * 0.5f },
					sx{ std::sin(dx) },
					cx{ std::cos(dx) },
					sy{ std::sin(dy) },
					cy{ std::cos(dy) },
					sz{ std::sin(dz) },
					cz{ std::cos(dz) },
					cc{ cy * cz },
					cs{ cy * sz },
					sc{ sy * cz },
					ss{ sy * sz };

				const std::array<float, 4> q
				{
					cc * sx + ss * cx,
					sc * cx - cs * sx,
					cs * cx - sc * sx,
					cc * cx + ss * sx
				};

				const std::array<float, 14> packet
				{
					m_position.x,
					m_position.y,
					m_position.z,
					1.f,
					m_position.x + -(q[0] * q[2] + (q[1] * q[3] * 2.f)),
					m_position.y + std::sin(m_rotation.x),
					m_position.z + -std::cos(m_rotation.x) * std::cos(m_rotation.y),
					1.f,
					q[0],
					q[1],
					q[2],
					q[3],
					0.f,
					m_fov
				};

				if (m_state == State::Battle)
				{
					m_ram.write(m_offset.battleCommonCamera - 0x40, packet); // Cutscene
					m_ram.write(m_offset.battleCommonCamera, packet);
				}
				else if (m_state == State::DragonMission)
				{
					const auto cameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };
					const auto cameraPtrState{ m_ram.read<u32>(cameraPtr + 0x244) };

					m_ram.write(cameraPtrState, packet);
				}
				else
				{
					const auto cameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr + 0x70) };

					m_ram.write(cameraPtr + 0x660, packet);
				}
			}
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_battleSetCameraPosition + 0xB4, 0x00000000, 0x0040F809,
			m_offset.Fn_battleSetQuaternion, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0020 },
			m_offset.Fn_battleUpdateCamera + 0x2C, 0x10000019, 0x54830019, // Cutscene
			m_offset.Fn_setCutsceneCameraPosition, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0xFFBF0050 },
			m_offset.Fn_MSNF_updateCamera + 0x100, 0x00000000, 0xF8810080, // Pos
			m_offset.Fn_MSNF_updateCamera + 0x28C, 0x00000000, 0xF8610080,  // Pos
			m_offset.Fn_MSNF_updateCamera + 0x180, 0x00000000, 0xE4610034,  // Fov
			m_offset.Fn_MSNF_updateCamera + 0xE8, 0x00000000, 0xF8610090 // Shaders
		);

		m_ram.writeConditional(m_state == State::DragonMissionFlying && m_isEnabled,
			m_offset.Fn_FLIGHT_updateCamera + 0x430,
				std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
				std::array<Mips_t, 4>{ 0xF85C0650, 0xF85D0660, 0xF85E0670, 0xF85F0680 },

			m_offset.Fn_FLIGHT_nextStep + 0xBC,
				std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
				std::array<Mips_t, 4>{ 0xF85C0000, 0xF85D0010, 0xF85E0020, 0xF85F0030 },

			m_offset.Fn_FLIGHT_cameraTransition + 0x288,
				std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
				std::array<Mips_t, 4>{ 0xF85C0000, 0xF85D0010, 0xF85E0020, 0xF85F0030 },

			m_offset.Fn_setProjectionMatrixFov + 0x50, 0x00000000, 0xE620003C,
			m_offset.Fn_setProjectionMatrix + 0x19C, 0x00000000, 0xE480003C
		);

		if (m_state != State::None)
		{
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

			if (m_state != State::DragonMissionCutscene && m_state != State::DragonMissionFlying)
			{
				MiscModel::updateTimescale(&m_input, Input::TimescalePos, Input::TimescaleNeg, &m_timescale, 1.25f, 
					timescaleMin * m_dep.timescaleBase, timescaleMax * m_dep.timescaleBase);
			}
			else
			{
				m_timescale = m_dep.timescaleBase;
			}
		}

		const bool isPauseMenuEnabled{ m_ram.read<s16>(m_offset.battleIsPauseMenuEnabled) == 1 };

		if (m_state == State::Battle)
		{
			m_ram.write(m_offset.battleTimescale, m_timescale);

			if (m_isGamePaused && !isPauseMenuEnabled)
			{
				m_ram.write(m_offset.battleIsPauseMenuEnabled + 0x14, s16(0)); // Type of menu
				m_ram.write(m_offset.battleIsPaused, 1);

				// Prevent "A supported controller is not connected in controller port 1..."
				SBuffer<0xBC> buffer;
				m_ram.read(m_offset.padData, buffer.data(), buffer.size());
				m_ram.write(m_offset.battlePadData, buffer);
			}
			else if (m_ram.read<s32>(m_offset.battleIsPaused + 0x10) == 4) // Tenkaichi Budokai Ring Out
			{
				m_ram.write(m_offset.battleIsPaused, 0);
			}
		}
		else if (m_state == State::DragonMission)
		{
			m_ram.write(m_offset.battleTimescale - 8, m_timescale);
		}

		m_ram.write(m_offset.Fn_battleUpdatePauseMenu + 0x66C, m_state == State::Battle && m_isGamePaused && !isPauseMenuEnabled ? 0x00000000 : 0xAC608B7C);
		m_ram.writeConditional(m_state == State::Battle && m_isHudHidden,
			m_offset.Fn_battleShouldDrawHud + 0x38, 0x00001021, 0x00000000,
			m_offset.Fn_battleDrawDuelingInfo + 0x58, 0x10000029, 0x10600029
		);

		const bool isNonBattleState{ m_state != State::Battle && m_state != State::None };

		m_ram.writeConditional(isNonBattleState && m_isGamePaused,
			m_offset.Fn_mdCutsceneNextStep + 0x250, 0x00000000, 0xE6000168, // Timer
			m_offset.Fn_MSN006_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN006_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN006_Upd, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x24050001 },
			m_offset.Fn_MSNF_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0xFFBF0080 },
			m_offset.Fn_MSNF_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF20, 0xFFBF00B0 },
			m_offset.Fn_MSN007_Mai, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x24060002 },
			m_offset.Fn_MSN007_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			m_offset.Fn_MSN007_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			m_offset.Fn_MSN010_PLY_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0xFFBF0050 },
			m_offset.Fn_MSN010_PLY_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			m_offset.Fn_mdKrilin_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF20, 0xFFBF0070 },
			m_offset.Fn_mdKrilin_PLY_PRE_EXEC + 0x24, 0x10000014, 0x18600014,
			m_offset.Fn_MSN014_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN014_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN014_updateBars, Mips::jrRaNop(), std::array<Mips_t, 2>{ m_dep.m014ubInstr, 0x24030001 },
			m_offset.Fn_MSN012_Mai, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x24040002 },
			m_offset.Fn_MSN012_Upd, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0x24040001 },
			m_offset.Fn_MSN012_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			m_offset.Fn_MSN012_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			m_offset.Fn_mdTenshinhan_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			m_offset.Fn_mdTenshinhan_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			m_offset.Fn_MSN013_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN013_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_MSN013_Upd + 0xD4, 0x1000002E, 0x5443002E, // Bars
			m_offset.Fn_MSN013_Upd + 0x198, 0x1000002D, 0x5444002D,
			m_offset.Fn_MSN013_updateLongBars, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFEA0, 0x24030001 },
			m_offset.Fn_MSN019_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			m_offset.Fn_MSN019_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0040 },
			m_offset.Fn_MSN019_UPDATE, Mips::jrRaNop(), std::array<Mips_t, 2>{ m_dep.m019uInstr, 0x3C02004B },
			m_offset.Fn_FLIGHT_PLAYER, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0030 },
			m_offset.Fn_FLIGHT_UPDATE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0050 },
			m_offset.Fn_FLIGHT_nextStep + 0x4C, 0x00000000, 0xE680000C, // Timer
			m_offset.Fn_FLIGHT_nextStep + 0xEC, 0x10000090, 0x12400090, // Next Step
			m_offset.Fn_SHOOT_update, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 }
		);

		m_ram.writeConditional(isNonBattleState && m_isHudHidden,
			m_offset.Fn_mdDrawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0xFFBF0040 },
			m_offset.Fn_drawSprites + 0x44, 0x10000018, 0x18400018,
			m_offset.Fn_MSNF_FIX_DRAW, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0040 },
			m_offset.Fn_FLIGHT_draw + 0x160, 0x10000030, 0x50400030 // Arrow
		);

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn_padStatus + 0x6C, 0xA623000E, 0xA620000E,
			m_offset.Fn_padStatus + 0x80, 0xA630000C, 0xA620000C
		);

		m_ram.writeConditional(m_isJoystickEnabled,
			m_offset.Fn_padStatus + 0xA4, 0xA2230018, 0xA2200018,
			m_offset.Fn_padStatus + 0xB0, 0xA2230019, 0xA2200019,
			m_offset.Fn_padStatus + 0xBC, 0xA223001A, 0xA220001A,
			m_offset.Fn_padStatus + 0xC8, 0xA223001B, 0xA220001B
		);
	}

	void Loop::updateBonus()
	{
		m_ram.writeConditional(m_displayMode,
			m_offset.Fn_initSelectStages + 0x38, 0x24040003, 0x0000202D,
			m_offset.Fn_drawAspectRatio + 0x5C, 0x24100003, 0x8F908A0C,
			m_offset.Fn_initMovies + 0x4C, 0x24040003, 0x0000202D,
			m_offset.Fn_initTitleScreen + 0x60, 0x24040003, 0x0000202D,
			m_offset.Fn_drawText + 0x204, 0x24040003, 0x0000202D,
			m_offset.Fn_drawGreenRectangle + 0xC, 0x24040003, 0x0000202D
		);

		m_ram.write(m_offset.Fn_drawAura,
			m_noAuras ? std::array<Mips_t, 2>{ 0x03E00008, 0x00001021 } : std::array<Mips_t, 2>{ 0x27BDFED0, 0x3C020055 });

		m_ram.write(m_offset.Fn_drawModel + 0xCC, m_noCelShading ? 0x00000000 : Mips::jal(m_offset.Fn_unknown2));
		m_ram.write(m_offset.Fn_setTextures + 0xCC, m_noShadersHighlight ? 0x00000000 : 0x14600024);

		auto noShaders = [&](bool enable)
		{
			m_ram.write(m_offset.Fn_setTextures + 0x290, enable ? 0x00000000 : Mips::jal(m_offset.Fn_unknown));
		};

		auto noTextures = [&](bool enable)
		{
			m_ram.write(m_offset.Fn_drawTexturesOnShaders, enable ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 });
		};

		switch (m_shadersMode)
		{
		case SHADERS_NORMAL:
			noShaders(false);
			noTextures(false);
			break;
		case SHADERS_NO_SHADERS:
			noShaders(true);
			noTextures(false);
			break;
		case SHADERS_NO_TEXTURES:
			noShaders(false);
			noTextures(true);
			break;
		}

		m_ram.writeConditional(m_noOnScreenEffects,
			m_offset.Fn_drawWhiteScreen + 0x2C, 0x00001821, 0x24030002,
			m_offset.Fn_drawWhiteScreen + 0x30, 0x00001821, 0x24030003,
			m_offset.Fn_drawGreyLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 }
		);

		if (m_unlockAll)
		{
			constexpr auto completion = []<u32 Size, u8 Data>()
			{
				std::array<u8, Size> arr;
				arr.fill(Data);
				return arr;
			};

			m_ram.write(m_offset.progression, completion.operator()<1103, 0xE0>()); // Capsules | Char | Stages | PadLock | Fighter's Road
			m_ram.write(m_offset.progression + 0x834, s8(1)); // Dragon Mission Continue
			m_ram.write(m_offset.progression + 0x44F, completion.operator()<269, 0xE1>()); // Dragon Mission | Fighter's Road

			Console::append(Console::Type::Success, "All unlocked successfully, to be effective you must save in settings");
			m_unlockAll = false;
		}
	}

	void Loop::enable(bool enable)
	{
		m_isEnabled = enable;
		m_timescale = m_dep.timescaleBase;

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
		}
	}
}
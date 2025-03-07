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

#include <cmath>
#include <type_traits>

namespace PS2::DBZBudokai3
{
	static constexpr auto duPosMultiplyScalar{ 32.f };
	static constexpr auto timescaleMin{ 0.1f }, timescaleMax{ 3.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::NtscU_GH)
		{
			d.dmShift = 0x7C;
			d.dmShift2 = 0xDC;
			d.ddduShift = 0xA0;
			d.ddmShift = 0x9C;
			d.dwsShift = 0x40;
			d.dwsShift2 = 0x30;
			d.daInstr = { 0x27BDFEA0, 0x3C02004D };
		}
		else
		{
			d.dmShift = 0x84;
			d.dmShift2 = 0xE8;
			d.ddduShift = 0xA4;
			d.ddmShift = 0x88;
			d.dwsShift = 0x30;
			d.dwsShift2 = 0x40;
			d.daInstr = { 0x27BDFEF0, 0x3C02BF80 };
		}

		if (version == Version::Pal || version == Version::PalCollector)
		{
			d.timescaleBase = 1.2f;
		}
		else
		{
			d.timescaleBase = 1.f;
		}

		if (version == Version::PalCollector || version == Version::NtscJ)
		{
			d.bscpShift = 0xBC;
			d.ducShift = 0x228;
			d.bucInstr = 0x14830018;
			d.bucInstr2 = 0x10000018;
			d.bupmShift = 0x674;
			d.bsdhShift = 0x3C;
			d.bddiShift = 0x54;
			d.bupmInstr = 0xAC20892C;
			d.bddiInstr = 0x10000010;
			d.bddiInstr2 = 0x10600010;
			
		}
		else if (version == Version::NtscU_GH)
		{
			d.bscpShift = 0xB8;
			d.ducShift = 0x220;
			d.bucInstr = 0x1083000B;
			d.bucInstr2 = 0x00000000;
			d.bupmShift = 0x6DC;
			d.bsdhShift = 0x38;
			d.bddiShift = 0x44;
			d.bupmInstr = 0xAC60892C;
			d.bddiInstr = 0x10000015;
			d.bddiInstr2 = 0x10600015;
		}
		else
		{
			d.bscpShift = 0xB4;
			d.ducShift = 0x228;
			d.bucInstr = 0x14830018;
			d.bucInstr2 = 0x10000018;
			d.bupmShift = 0x664;
			d.bsdhShift = 0x3C;
			d.bddiShift = 0x54;
			d.bupmInstr = 0xAC2088EC;
			d.bddiInstr = 0x10000010;
			d.bddiInstr2 = 0x10600010;
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
				JSON_GET(j, m_noShadersHighlight);
				JSON_GET_MIN_MAX(j, m_shadersMode, 0, SHADERS_COUNT - 1);
				JSON_GET(j, m_noOnScreenEffects);
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_infFarClip);
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
		JSON_SET(j, m_noShadersHighlight);
		JSON_SET(j, m_shadersMode);
		JSON_SET(j, m_noOnScreenEffects);
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_infFarClip);
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
		const auto cameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };

		if (PS2::isValidMemoryRange(cameraPtr) &&
			PS2::isValidMemoryRange(m_ram.read<u32>(cameraPtr + 0x260)))
		{
			m_state = State::Battle;
		}
		else if (PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.duBackgroundPtr)))
		{
			m_state = State::DragonUniverse;
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

		if (m_state == State::DragonUniverse)
		{
			posSpeed *= duPosMultiplyScalar;
		}

		CameraModel::drawPosition(&m_position, posSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Loop::drawOthers()
	{
		if (m_state == State::DragonUniverse)
		{
			MiscModel::drawMiscHideHud(&m_isHudHidden, !m_isEnabled);
		}
		else
		{
			MiscModel::drawTimescale(&m_timescale, timescaleMin * m_dep.timescaleBase, timescaleMax * m_dep.timescaleBase, !m_isEnabled);
			MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
		}

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
		if (m_version != Version::NtscU_GH)
		{
			Ui::checkbox(Ui::lol("16:9"), &m_displayMode);
			Ui::hoveredTooltip("Turn the game in 16:9 aspect ratio, this mode is official but unused / unfinished");
		}

		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Cel Shading"), &m_noCelShading);
		Ui::checkbox(Ui::lol("No Shaders Highlight"), &m_noShadersHighlight);
		ImGui::Combo(Ui::lol("Shaders"), &m_shadersMode, "Normal\0No Shaders\0No Textures\0");

		Ui::separatorText("Battle");
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/grey lines");

		Ui::separatorText("Dragon Universe");
		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("Infinite Far Distance"), &m_infFarClip);
		Ui::hoveredTooltip("Remove the far clip limit\n*Showing an important part of the map (Ex: using a high fov) may crash the game");

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

			if (m_state == State::DragonUniverse)
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

			if (m_state == State::Battle)
			{
				const auto cameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };
				const auto cameraPtrState{ m_ram.read<u32>(cameraPtr + 0x264) };

				m_ram.read(cameraPtrState, &m_position);
				m_ram.read(cameraPtrState + 0x34, &m_fov);
				m_ram.read(cameraPtr + 0x60, &vm);
			}
			else
			{
				m_ram.read(m_offset.duPosition, &m_position);
				m_ram.read(m_offset.duPosition + 0x50, &vm);
				m_fov = 448.f / 512.f / 2.f;
			}

			m_rotation.x = std::asin(-vm[1][2]);
			m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
			m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
		};

		auto write = [&]()
		{
			if (m_state == State::Battle)
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

				m_ram.write(m_offset.battleCommonCamera - 0x40, packet); // Cutscene
				m_ram.write(m_offset.battleCommonCamera, packet);
			}
			else
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

				m_ram.write(m_offset.duPosition + 0x50, vm);
				m_ram.write(m_offset.globalFov, 448.f / (m_fov * 2.f));
			}
		};

		if (!m_isEnabled)
		{
			m_ram.write(m_offset.globalFov, 512.f);
		}

		m_ram.write(m_offset.Fn_drawModel, m_state == State::DragonUniverse && m_isEnabled ?
			Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 });

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_battleSetCameraPosition + m_dep.bscpShift, 0x00000000, 0x0040F809,
			m_offset.Fn_battleSetQuaternion, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0020 },
			m_offset.Fn_battleUpdateCamera + 0x2C, m_dep.bucInstr2, m_dep.bucInstr, // Cutscene
			m_offset.Fn_duUpdateCamera + m_dep.ducShift,
				std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
				std::array<Mips_t, 4>{ 0xF85C0000, 0xF85D0010, 0xF85E0020, 0xF85F0030 }
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
			MiscModel::toggle(&m_input, Input::HideHud, &m_isHudHidden);
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(&m_input, Input::Joystick, &m_isJoystickEnabled);

			if (m_state != State::DragonUniverse)
			{
				MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
				MiscModel::updateTimescale(&m_input, Input::TimescalePos, Input::TimescaleNeg, &m_timescale, 1.25f,
					timescaleMin * m_dep.timescaleBase, timescaleMax * m_dep.timescaleBase);
			}
			else
			{
				m_isGamePaused = false;
				m_timescale = m_dep.timescaleBase;
			}
		}

		const bool isPauseMenuEnabled{ m_ram.read<s16>(m_offset.battleIsPauseMenuEnabled) == 1 };

		m_ram.write(m_offset.Fn_battleUpdatePauseMenu + m_dep.bupmShift, 
			m_state == State::Battle && m_isGamePaused && !isPauseMenuEnabled ? 0x00000000 : m_dep.bupmInstr);

		m_ram.writeConditional(m_state == State::Battle && m_isHudHidden,
			m_offset.Fn_battleShouldDrawHud + m_dep.bsdhShift, 0x00001021, 0x00000000,
			m_offset.Fn_battleDrawDuelingInfo + m_dep.bddiShift, m_dep.bddiInstr, m_dep.bddiInstr2
		);

		m_ram.writeConditional(m_state == State::DragonUniverse && m_isHudHidden,
			m_offset.Fn_duACTDRAW + 4, std::array<Mips_t, 2>{ 0x03E00008, 0x27BD0060 }, std::array<Mips_t, 2>{ 0xFFBF0020, 0x7FB10010 },
			m_offset.Fn_duMAP, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0x24030002 },
			m_offset.Fn_duBALL_DRAW, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x3C020003 },
			m_offset.Fn_duPOS, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF80, 0x24030002 }
		);

		if (m_state == State::Battle)
		{
			m_ram.write(m_offset.battleTimescale, m_timescale);

			if (m_isGamePaused && !isPauseMenuEnabled)
			{
				m_ram.write(m_offset.battleIsPauseMenuEnabled + 0x14, s16(0)); // Type of menu
				m_ram.write(m_offset.battleIsPaused, 1);
			}
			else if (m_ram.read<s32>(m_offset.battleIsPaused + 0xC) == 5) // Tenkaichi Budokai Ring Out
			{
				m_ram.write(m_offset.battleIsPaused, 0);
			}
		}

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn_padStatus + 0x60, 0xA623000E, 0xA620000E,
			m_offset.Fn_padStatus + 0x70, 0xA624000C, 0xA620000C
		);

		m_ram.writeConditional(m_isJoystickEnabled,
			m_offset.Fn_padStatus + 0x98, 0xA2230018, 0xA2200018,
			m_offset.Fn_padStatus + 0xA4, 0xA2230019, 0xA2200019,
			m_offset.Fn_padStatus + 0xB0, 0xA223001A, 0xA220001A,
			m_offset.Fn_padStatus + 0xBC, 0xA223001B, 0xA220001B
		);
	}

	void Loop::updateBonus()
	{
		if (m_version != Version::NtscU_GH)
		{
			m_ram.writeConditional(m_displayMode,
				m_offset.displayMode, 1, 0,
				m_offset.displayMode2, 1, 0
			);
		}

		m_ram.write(m_offset.Fn_drawAura, m_noAuras ? std::array<Mips_t, 2>{ 0x03E00008, 0x00001021 } : m_dep.daInstr);
		m_ram.write(m_offset.Fn_drawModel + m_dep.dmShift, m_noCelShading ? 0x00003821 : 0x34470200);
		m_ram.write(m_offset.Fn_setTextures + 0xE4, m_noShadersHighlight ? 0x00000000 : 0x14600024);

		auto noShaders = [&](bool enable)
		{
			m_ram.write(m_offset.Fn_setTextures + 0x2B0, enable ? 0x00000000 : Mips::jal(m_offset.Fn_unknown));
		};

		auto noTextures = [&](bool enable)
		{
			m_ram.write(m_offset.Fn_drawModel + m_dep.dmShift2, enable ? 0x00002021 : 0x3C042000);
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
			m_offset.Fn_drawWhiteScreen + m_dep.dwsShift, 0x00001821, 0x24030003,
			m_offset.Fn_drawWhiteScreen + m_dep.dwsShift2, 0x00001821, 0x24030002,
			m_offset.Fn_drawGreyLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 }
		);

		m_ram.write(m_offset.Fn_duDrawDragonUniverse + m_dep.ddduShift, m_noFog ? 0x44800000 : 0xC6000008);
		m_ram.write(m_offset.Fn_duDrawMap + m_dep.ddmShift, m_infFarClip ? 0x3C02487A : 0x3C02457A);

		if (m_unlockAll)
		{
			constexpr auto allCapsules = []()
			{
				std::array<u8, 579> capsules;
				capsules.fill(u8(9));
				return capsules;
			};

			static constexpr u64 allChar{ 0x00'00'0F'FF'78'3F'FF'FF };

			m_ram.write(m_offset.progression + 8, allChar);
			m_ram.write(m_offset.progression + 0xB8, allChar); // DU
			m_ram.write(m_offset.progression + 0x10, u16(0x0FFF)); // Stage
			m_ram.write(m_offset.progression + 0xA9, s8(1)); // Dragon Arena
			m_ram.write(m_offset.capsules, allCapsules());

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
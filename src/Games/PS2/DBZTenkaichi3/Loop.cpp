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

namespace PS2::DBZTenkaichi3
{
	using Fn_SetViewMatrix = std::array<Mips_t, 9>;

	static constexpr auto viewMatrixSize{ sizeof(float[4][4]) };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.cucShift = 0x4AC;
			d.usInstr = 0x1460001D;
			d.dwlInstr = 0x8F82AB80;
		}
		else
		{
			d.cucShift = 0x4B4;
			d.usInstr = 0x1460001F;
			d.dwlInstr = 0x8F82A800;
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
				JSON_GET(j, m_noBlur);
				JSON_GET(j, m_noAuras);
				JSON_GET(j, m_noNearTransparency);
				JSON_GET(j, m_noOnScreenEffects);
				JSON_GET(j, m_shaders);
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
		JSON_SET(j, m_noBlur);
		JSON_SET(j, m_noAuras);
		JSON_SET(j, m_noNearTransparency);
		JSON_SET(j, m_noOnScreenEffects);
		JSON_SET(j, m_shaders);
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
		const auto [vmo1, vmo2]{ Mips::li32(Mips::Register::a1, viewMatrixOffset()) };

		const Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x7FBF0000, // sq ra,(sp)
			vmo1,
			vmo2,
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		m_ram.write(m_offset.Fn_unknown, Fn_setViewMatrix);

		const auto cutscenePtr{ m_ram.read<u32>(m_offset.battlePlayerPtr + 0xB8) };

		if (PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.battlePlayerPtr)))
		{
			if (m_ram.read<s32>(m_offset.splitscreen) == 0 || m_ram.read<s32>(m_offset.replayMode) == 1)
			{
				m_state = State::Battle;

				if (PS2::isValidMemoryRange(cutscenePtr))
				{
					if (m_ram.read<s32>(cutscenePtr + 0x32C) == 1)
					{
						m_state = State::DragonHistory;
					}
					else if (PS2::isValidMemoryRange(m_ram.read<u32>(cutscenePtr + 0x2C0)))
					{
						m_state = State::BattleCutscene;
					}
				}
			}
			else
			{
				m_state = State::None;
			}
		}
		else if (const auto viewModelPtr{ m_ram.read<u32>(m_offset.viewModelPtr) };
			PS2::isValidMemoryRange(viewModelPtr) && m_ram.read<s32>(viewModelPtr + 0x1628) == 7)
		{
			m_state = State::ViewModel;
		}
		else if (PS2::isValidMemoryRange(cutscenePtr))
		{
			m_state = m_ram.read<s32>(cutscenePtr + 0x32C) == 1 ? State::ShenronWish : State::ShenronCutscene;
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
		FreecamModel::draw3DSettingsNoFov();
	}

	void Loop::drawCamera()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
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
	}

	void Loop::drawBonus()
	{
		Ui::setXSpacingStr("No Near Transparency");

		Ui::checkbox(Ui::lol("No Blur"), &m_noBlur);
		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Near Transparency"), &m_noNearTransparency);
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/lines");
		Ui::slider(Ui::lol("Shaders"), &m_shaders, "%d", ImGuiSliderFlags_AlwaysClamp);

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
			FreecamModel::update3DSettingsNoFov(&m_input,
				Input::MovementSpeedPos, Input::MovementSpeedNeg,
				Input::RotationSpeedPos, Input::RotationSpeedNeg);

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
				fv{ m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward) },
				rv{ m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft) },
				uv{ m_controls.upVelocity(Input::MoveUp, Input::MoveDown) };

			m_position.x += cx * sy * fv;
			m_position.y += -sx * fv;
			m_position.z += cy * cx * fv;
			m_position.x += (cc + ss * sx) * rv;
			m_position.y += cx * sz * rv;
			m_position.z += (cs * sx - sc) * rv;
			m_position.x -= (sc * sx - cs) * uv;
			m_position.y -= cx * cz * uv;
			m_position.z -= (cc * sx + ss) * uv;

			CameraModel::rotatePitch(&m_rotation.x, m_controls.pitchVelocity(Input::RotateXPos, Input::RotateXNeg));
			CameraModel::rotateYaw(&m_rotation.y, m_controls.yawVelocity(Input::RotateYPos, Input::RotateYNeg));
			CameraModel::rotateRoll(&m_rotation.z, m_controls.rollVelocity(Input::RotateZPos, Input::RotateZNeg));
		}
	}

	void Loop::updateCamera()
	{
		auto read = [&]()
		{
			const auto [vmPtr, posPtr]{ vmPosPtr() };

			if (!vmPtr || !posPtr)
			{
				return;
			}

			float vm[4][4];
			m_ram.read(vmPtr, &vm);
			m_ram.read(posPtr, &m_position);
			m_rotation.x = std::asin(-vm[1][2]);
			m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
			m_rotation.z = std::atan2(vm[1][0], vm[1][1]);

			if (m_state == State::BattleCutscene)
			{
				const std::array<float, 3> vec{ m_position.x, m_position.y, m_position.z };
				m_position = {};
				m_ram.read(posPtr - 0x40, &vm);

				for (s32 i{}; i < 3; ++i)
				{
					m_position.x += vm[i][0] * vec[i];
					m_position.y += vm[i][1] * vec[i];
					m_position.z += vm[i][2] * vec[i];
				}

				m_position.x += vm[3][0];
				m_position.y += vm[3][1];
				m_position.z += vm[3][2];
			}
		};

		auto write = [&]()
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

			m_ram.write(viewMatrixOffset(), vm, viewMatrixSize);
		};

		const auto
			jal_setViewMatrix{ Mips::jal(m_offset.Fn_setViewMatrix) },
			jal_ccSetViewMatrix{ Mips::jal(m_offset.Fn_unknown) },
			jal_sceVu0InversMatrix{ Mips::jal(m_offset.Fn_sceVu0InversMatrix) };

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_battleSetPlayerCamera + 0x28, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x26040040 }, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x26050040 },
			m_offset.Fn_viewModelUpdateCamera + 0x280, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x00A02021 }, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x0200202D },
			// Story - Shenron
			m_offset.Fn_cutsceneUpdateCamera + 0x140, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x24840040 }, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x24850040 },
			// Cutscene
			m_offset.Fn_cutsceneUpdateCamera + m_dep.cucShift, jal_ccSetViewMatrix, jal_sceVu0InversMatrix,
			m_offset.Fn_cutsceneUpdateCamera + m_dep.cucShift + 0xD4, 0x00000000, jal_sceVu0InversMatrix
		);

		m_isEnabled ? write() : read();
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

		const auto flags{ m_ram.read<s32>(m_offset.flags) };
		const bool isPauseMenuEnabled{ (m_state == State::Battle || m_state == State::BattleCutscene) && flags & 0x4000 ? true : false };

		if (m_state != State::None)
		{
			m_ram.write(m_offset.flags, m_isGamePaused || isPauseMenuEnabled ? flags | 0x100 : flags & ~0x100);
		}

		m_ram.writeConditional(m_state != State::None && m_isGamePaused,
			m_offset.Fn_updateChar + 0x24, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600056, 0xDFB00020 }, // Frustrum
			m_offset.Fn_updateChar + 0xF0, 0x10000011, 0x14400011, // Anim
			m_offset.Fn_updateCharBlinkEyes, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB10008 },
			m_offset.Fn_updateCharVisibility + 0x30, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600029, 0xDFB00000 },
			m_offset.Fn_updateCharVisibility2 + 0x30, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600035, 0xDFB00000 }
		);

		if (m_version != Version::NtscJ)
		{
			m_ram.write(m_offset.Fn_updateShenron + 0x68, m_state != State::None && m_isGamePaused ? 0x00000000 : m_dep.usInstr);
		}

		m_ram.writeConditional(m_state != State::None && m_isHudHidden,
			m_offset.Fn_battleDrawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			m_offset.Fn_drawSprite, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFB00000 }
		);

		m_ram.write(m_offset.Fn_padStatus + 0x2F0, m_isButtonEnabled ? 0xAE300148 : 0xAE200148);

		m_ram.writeConditional(m_isJoystickEnabled,
			m_offset.Fn_padStatus + 0x108, 0xE6200138, 0xAE200138,
			m_offset.Fn_padStatus + 0x118, 0xE620013C, 0xAE20013C,
			m_offset.Fn_padStatus + 0x128, 0xE6200130, 0xAE200130,
			m_offset.Fn_padStatus + 0x138, 0xE6200134, 0xAE200134
		);
	}

	void Loop::updateBonus()
	{
		m_ram.writeConditional(m_noBlur,
			m_offset.Fn_drawFarBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF40, 0x24050010 },
			m_offset.Fn_drawNearBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 }
		);

		m_ram.writeConditional(m_noAuras,
			m_offset.Fn_drawAuras, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 },
			m_offset.Fn_drawAurasLightning, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 }
		);

		m_ram.write(m_offset.Fn_updateCharNear + 0xB4, m_noNearTransparency ? 0x1000002C : 0x1040002C);

		m_ram.writeConditional(m_noOnScreenEffects,
			m_offset.Fn_drawWhiteScreen, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C02002F },
			m_offset.Fn_drawShade, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB20010 },
			m_offset.Fn_drawWhiteLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFDB0, m_dep.dwlInstr },
			m_offset.Fn_drawBlurCutscene, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00A0382D, 0x3C05FF00 }
		);

		m_ram.write(m_offset.Fn_drawCharTextures + 0x418, Mips::li(Mips::Register::a1, m_shaders));

		if (m_unlockAll)
		{
			const auto progressionPtr{ m_ram.read<u32>(m_offset.progressionPtr) };

			if (progressionPtr)
			{
				static constexpr std::array<u8, 132> progression
				{
					0x3D, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
					0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00,
					0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
					0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
					0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
					0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x01, 0x00, 0x00, 0x00
				};

				static constexpr std::array<u8, 32> progression2
				{
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Char 1
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Char 2
					0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, // Char 3
					0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00  // Stage
				};

				static constexpr std::array<u8, 152> items
				{
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
				};

				struct ExpLevel
				{
					s32 exp;
					u16 level;
				};

				static constexpr std::array<ExpLevel, 97> expLevel
				{
					20000, 4, 15000, 5, 20000, 4, 20000, 4,
					16000, 4, 20000, 4, 20000, 4, 20000, 4,
					20000, 4, 21000, 3, 20000, 4, 19000, 4,
					15000, 5, 19000, 4, 21000, 3, 20000, 4,
					21000, 3, 21000, 3, 25000, 3, 15000, 5,
					15000, 5, 20000, 4, 20000, 4, 18000, 4,
					15000, 5, 20000, 4, 20000, 4, 15000, 5,
					15000, 5, 15000, 5, 20000, 4, 20000, 4,
					18000, 4, 19000, 4, 15000, 5, 20000, 4,
					19000, 4, 20000, 4, 16000, 4, 15000, 5,
					16000, 4, 19000, 4, 25000, 3, 15000, 3,
					18000, 4, 18000, 4, 15000, 5, 25000, 3,
					19000, 4, 25000, 5, 20000, 4, 24000, 4,
					24000, 4, 15000, 3, 15000, 3, 15000, 3,
					19000, 4, 20000, 4, 21000, 3, 25000, 2,
					15000, 5, 19000, 4, 19000, 4, 20000, 4,
					18000, 4, 25000, 3, 24000, 4, 19000, 4,
					20000, 4, 16000, 4, 25000, 3, 15000, 3,
					15000, 3, 25000, 2, 18000, 4, 25000, 3,
					25000, 3, 20000, 4, 20000, 4, 25000, 3,
					24000, 4, 25000, 2, 25000, 2, 25000, 2,
					25000, 3, 18000, 4, 18000, 4, 20000, 4,
					18000, 4, 18000, 4, 24000, 4, 15000, 5,
					15000, 5, 15000, 5, 20000, 4, 24000, 4,
					21000, 3
				};

				static constexpr auto evoZSize{ 56 * 97 };
				Buffer charData(evoZSize);
				auto* const charDataPtr{ charData.data() };
				m_ram.read(progressionPtr + 0x1808, charDataPtr, evoZSize);

				for (s32 i{}; i < 97; ++i)
				{
					auto* const expPtr{ charDataPtr + i * 56 + 0x30 };
					*(s32*)expPtr = expLevel[i].exp;
					*(u16*)(expPtr + sizeof(s32)) = expLevel[i].level;
				}

				m_ram.write(progressionPtr + 8, 0x000001FF); // Dragon Ball | Skip Dragon History Goku Dialog
				m_ram.write(progressionPtr + 0x10, progression); // Dragon History
				m_ram.write(progressionPtr + 0x208, 0x0000001400000001); // Ultimate Battle Mission 100 | Survival
				m_ram.write(progressionPtr + 0xC10, progression2);
				m_ram.write(progressionPtr + 0x1808, *charDataPtr, evoZSize);
				m_ram.write(progressionPtr + 0x2EC8, items);

				if (m_version == Version::NtscJ)
				{
					m_ram.write(progressionPtr + 0xC30, 0x01FFFFFF); // Ost
				}

				Console::append(Console::Type::Success, "All unlocked successfully");
			}
			else
			{
				Console::append(Console::Type::Error, "Can't unlock all");
			}

			m_unlockAll = false;
		}
	}

	void Loop::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			FreecamModel::resetSpeed(m_resetMovementSpeed, m_resetRotationSpeed, false);
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

	std::pair<u32, u32> Loop::vmPosPtr() const
	{
		auto get = [](u32 vm, u32 vmShift, u32 pos, u32 posShift) -> std::pair<u32, u32>
		{
			if (vm && pos)
			{
				return { vm + vmShift, pos + posShift };
			}
			return { 0, 0 };
		};

		if (m_state == State::Battle)
		{
			const auto player1Ptr{ m_ram.read<u32>(m_offset.battlePlayerPtr) + 4 };
			const auto posShift{ m_ram.read<s32>(player1Ptr + 0x130) == 0 ? 0x430 : 0x1A30 };
			return get(m_ram.read<u32>(m_offset.battlePlayerPtr + 0xBC), 0x40, m_ram.read<u32>(player1Ptr), posShift);
		}
		else if (m_state == State::BattleCutscene || m_state == State::ShenronCutscene)
		{
			const auto ptr{ m_ram.read<u32>(m_offset.battlePlayerPtr + 0xB8) };
			return get(ptr, 0x40, ptr, 0x2A0);
		}
		else if (m_state == State::DragonHistory || m_state == State::ShenronWish)
		{
			const auto ptr{ m_ram.read<u32>(m_offset.battlePlayerPtr + 0xB8) };
			return get(ptr, 0x40, ptr, 0x2D0);
		}
		else if (m_state == State::ViewModel)
		{
			return get(m_ram.read<u32>(m_offset.battlePlayerPtr + 0xBC), 0x40, m_ram.read<u32>(m_offset.battlePlayerPtr + 0xC8), 0);
		}

		return { 0, 0 };
	}

	u32 Loop::viewMatrixOffset() const
	{
		return m_offset.Fn_unknown + sizeof(Fn_SetViewMatrix);
	}
}
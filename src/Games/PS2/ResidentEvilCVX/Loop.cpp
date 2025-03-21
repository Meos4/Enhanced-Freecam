#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/Math.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>
#include <cmath>
#include <type_traits>

namespace PS2::ResidentEvilCVX
{
	using Fn_SetViewMatrix = std::array<Mips_t, 9>;

	static constexpr auto viewMatrixSize{ sizeof(float[4][4]) };

	Loop::Loop(Ram&& ram, s32 version)
		: m_ram(std::move(ram)),
		m_version(version),
		m_offset(Offset::create(version)),
		m_input(&Game::baseInputs),
		m_controls(&m_input)
	{
		const auto jsonRead{ Json::read(PS2::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			try
			{
				const auto& j{ jsonRead.value() };
				JSON_GET(j, m_resetZRotation);
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noCutsceneBlackBars);
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
		JSON_SET(j, m_disableButton);
		JSON_SET(j, m_disableJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noCutsceneBlackBars);
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

		m_ram.write(m_offset.Fn__s2b, Fn_setViewMatrix);

		const auto [f1, f2]{ m_ram.read<std::array<s32, 2>>(m_offset.sysp + 0x54) };

		static constexpr auto 
			bhSysCallGame{ 0x00000080 },
			bhSysCallDoordemo{ 0x00000800 };

		if ((f1 & bhSysCallGame) && !(f2 & bhSysCallGame))
		{
			m_state = State::Common;
		}
		else if ((f1 & bhSysCallDoordemo) && !(f2 & bhSysCallDoordemo))
		{
			m_state = State::Door;
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
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Loop::drawOthers()
	{
		if (m_state != State::Door)
		{
			MiscModel::drawMiscPauseGame(&m_isGamePaused, !m_isEnabled);
		}
		
		MiscModel::drawControllerButtonJoystick(&m_isButtonEnabled, &m_isJoystickEnabled, !m_isEnabled);
	}

	void Loop::drawSettings()
	{
		Ui::setXSpacingStr("Reset Movement Speed");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Reset Z Rotation"), &m_resetZRotation);
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Disable Joystick"), &m_disableJoystick);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
	}

	void Loop::drawBonus()
	{
		Ui::setXSpacingStr("No Cutscene Black Bars");

		Ui::checkbox(Ui::lol("No Cutscene Black Bars"), &m_noCutsceneBlackBars);
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

			CameraModel::rotatePitch(&m_rotation.x, m_fov, m_controls.pitchVelocity(Input::RotateXPos, Input::RotateXNeg));
			CameraModel::rotateYaw(&m_rotation.y, m_fov, m_controls.yawVelocity(Input::RotateYPos, Input::RotateYNeg));
			CameraModel::rotateRoll(&m_rotation.z, m_controls.rollVelocity(Input::RotateZPos, Input::RotateZNeg));
			CameraModel::increaseFov(&m_fov, m_controls.fovVelocity(Input::FovPos, Input::FovNeg));
		}
	}

	void Loop::updateCamera()
	{
		constexpr auto floatFov = [](s32 fov)
		{
			return static_cast<float>(fov) / 32767 * Math::pi;
		};

		auto intFov = [](float fov)
		{
			return static_cast<s32>(fov / Math::pi * 32767);
		};

		auto read = [&]()
		{
			if (m_state == State::Common)
			{
				const auto vmPtr{ m_ram.read<u32>(m_offset.cam + 0x9C) };
				if (vmPtr)
				{
					float vm[4][4];
					m_ram.read(vmPtr, &vm);
					m_rotation.x = std::asin(-vm[1][2]);
					m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
					m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
					m_ram.read(m_offset.cam + 0xC, &m_position);
					m_position += m_ram.read<Vec3<float>>(m_offset.cam + 0x48);
					m_fov = floatFov(m_ram.read<s32>(m_offset.cam + 0x84));
					const auto forward{ m_ram.read<float>(m_offset.cam + 0x6C) };

					if (forward)
					{
						const auto
							sx{ std::sin(m_rotation.x) },
							cx{ std::cos(m_rotation.x) },
							sy{ std::sin(m_rotation.y) },
							cy{ std::cos(m_rotation.y) };

						m_position.x -= cx * sy * forward;
						m_position.y -= -sx * forward;
						m_position.z -= cy * cx * forward;
					}
				}
			}
			else
			{
				m_ram.read(m_offset.DoorWrk + 0x58, &m_position);
				const auto [rx, ry, rz]{ m_ram.read<Vec3<s32>>(m_offset.DoorWrk + 0x70) };
				m_rotation = { -floatFov(rx), -floatFov(ry), -floatFov(rz) };
				m_rotation.y += Math::pi;
				m_rotation.z += Math::pi;
				m_fov = floatFov(0x31C7);
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
			vm[0][3] = vm[1][3] = vm[2][3] = vm[3][3] = 0.f;
	
			m_ram.write(viewMatrixOffset(), vm, viewMatrixSize);
			m_ram.write(m_offset.cam + 0x84, intFov(m_fov));
			writeNearFarClip(-2.f, -20000.f);
		};

		const auto liFov{ Mips::li(Mips::Register::a0, static_cast<u16>(intFov(m_fov))) };

		m_ram.writeConditional(m_isEnabled,
			// Common Vm
			m_offset.Fn_bhInitCamera + 0x84, 0x00000000, Mips::jal(m_offset.Fn_njUnitMatrix),
			// Common Fov
			m_offset.Fn_bhControlActiveCamera + 0xBE8, 0x00000000, 0xAC239114,
			m_offset.Fn_bhInitCamera + 0x38, 0x00000000, 0xAC229114,
			m_offset.Fn_bhSetCut + 0x434, 0x00000000, 0xAC239114,
			m_offset.Fn_bhInitActiveCamera + 0x6BC, 0x00000000, 0xAC239114,
			m_offset.Fn_bhSetEventCamera + 0x170, 0x00000000, 0xAC229114,
			m_offset.Fn_bhControlEventCamera + 0x454, 0x00000000, 0xAC229114,
			m_offset.Fn_bhControlEventCamera + 0x9A4, 0x00000000, 0xAC229114,
			// Door Fov
			m_offset.Fn_bhControlDoor + 0x28C, liFov, 0x240431C7,
			m_offset.Fn_bhControlDoor + 0x298, liFov, 0x240431C7
		);

		if (m_state != State::None)
		{
			m_isEnabled ? write() : read();
		}

		auto readCurrentVmMbufPtr = [](u32 offset)
		{
			std::array<Mips_t, 2> instructions
			{ 
				Mips::lui(Mips::Register::a0, static_cast<u16>(offset >> 16)),
				instructions[1] = 0x8C840000 + static_cast<u16>(offset)
			};

			if ((offset & 0x0000FFFF) > 0x7FFF)
			{
				instructions[0] += 1;
			}

			return instructions;
		};

		const auto [ins1, ins2]{ readCurrentVmMbufPtr(m_offset.pNaMatMatrixStuckPtr) };

		const std::array<Mips_t, 3>
		doorTranslateCall
		{
			0x46000B47,
			Mips::jal(m_offset.Fn_njTranslate),
			0x46000387
		},
		setViewMatrixCall
		{
			ins1,
			Mips::jal(m_offset.Fn__s2b),
			ins2
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_bhControlCamera + 0xE0, Mips::jal(m_offset.Fn__s2b), Mips::jal(m_offset.Fn_njGetMatrix),
			m_offset.Fn_bhControlDoor + 0x254, setViewMatrixCall, doorTranslateCall
		);
	}

	void Loop::updateOthers()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(&m_input, Input::Joystick, &m_isJoystickEnabled);

			if (m_state != State::Door)
			{
				MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
			}
			else 
			{
				m_isGamePaused = false;
			}
		}

		m_ram.writeConditional(m_state == State::Common && m_isGamePaused,
			m_offset.Fn_bhControlPlayer + 0x34, 0x10000434, 0x10600434,
			m_offset.Fn_bhControlEffect + 0x20, 0x100000AF, 0x14600006,
			m_offset.Fn_bhControlObjTim + 0x28, 0x10000177, 0x10600177,
			m_offset.Fn_bhControlEnemy + 0x38, 0x10000059, 0x10600059,
			m_offset.Fn_bhControlLight + 0x264, 0x100002D1, 0x104002D1,
			m_offset.Fn_bhControlLight + 0x1280, 0x1000000D, 0x1040000D,
			m_offset.Fn_bhEff106 + 0x7C, 0x10000062, 0x10400062,
			m_offset.Fn_bhSysCallEvent + 0x350, 0x00000000, Mips::jal(m_offset.Fn_bhControlEvent)
		);

		static constexpr std::array<Mips_t, 3> 
		vanillaStick
		{
			0x8C620000,
			0x72002628,
			0x00401027
		},
		defaultStick
		{
			0x3C028080,
			0x72002628,
			0x34428080
		};

		m_ram.write(m_offset.Fn_Ps2_pad_read + 0x44C, m_isButtonEnabled ? 0xA4E20000 : 0xA4E00000);
		m_ram.write(m_offset.Fn_Ps2_pad_read + 0x450, m_isJoystickEnabled ? vanillaStick : defaultStick);
	}

	void Loop::updateBonus()
	{
		const auto jal_njDrawPolygon2D{ Mips::jal(m_offset.Fn_njDrawPolygon2D) };

		m_ram.writeConditional(m_noCutsceneBlackBars,
			m_offset.Fn_bhDrawCinesco + 0x100, 0x00000000, jal_njDrawPolygon2D,
			m_offset.Fn_bhDrawCinesco + 0x134, 0x00000000, jal_njDrawPolygon2D
		);
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
				m_rotation.z = Math::toRadians(180.f);
			}
			if (m_pauseGame)
			{
				m_isGamePaused = true;
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
			m_isButtonEnabled = true;
			m_isJoystickEnabled = true;

			const auto [_near, padding, _far]{ m_ram.read<std::array<float, 3>>(m_offset.fNaViwClipNear) };
			writeNearFarClip(_near, _far);
		}
	}

	void Loop::writeNearFarClip(float _near, float _far) const
	{
		const auto range{ _far - _near };
		static constexpr auto z{ 65536.f };

		m_ram.write(m_offset.fNaViwClipNear + 0x10, -_near); // _fNaViwClipNear
		m_ram.write(m_offset.fNaViwClipNear + 0x18, -_far); // _fNaViwClipFar
		m_ram.write(m_offset.Ps2_zbuff_a, z * _near * _far / range);
		m_ram.write(m_offset.Ps2_zbuff_a + 8, -z * _near / range); // Ps2_zbuff_b
		m_ram.write(m_offset.fVu1FarClip, -_far);
		m_ram.write(m_offset.fVu1FarClip + 8, 1.f / -_near); // fVu1InvNearClip
		m_ram.write(m_offset.fVu1FarClip + 0x10, -_near); // fVu1NearClip
	}

	u32 Loop::viewMatrixOffset() const
	{
		return m_offset.Fn__s2b + sizeof(Fn_SetViewMatrix);
	}
}
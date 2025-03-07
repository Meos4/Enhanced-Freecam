#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <cmath>
#include <type_traits>

namespace PS2::ResidentEvil4
{
	using Fn_SetPacket = std::array<Mips_t, 22>;
	using Fn_SetFov = std::array<Mips_t, 5>;

	static constexpr auto posMultiplyScalar{ 32.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::NtscJ)
		{
			d.upShift = 0x4B0;
		}
		else
		{
			d.upShift = 0x4AC;
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
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_noGameOver);
				JSON_GET(j, m_noCollisions);
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
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_noGameOver);
		JSON_SET(j, m_noCollisions);
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
		const auto [cp1, cp2]{ Mips::li32(Mips::Register::t1, m_offset.cameraPtr) };
		const auto [po1, po2]{ Mips::li32(Mips::Register::t2, packetOffset()) };
		const auto [fo1, fo2]{ Mips::li32(Mips::Register::t0, fovOffset()) };

		const Fn_SetPacket Fn_setPacket
		{
			0x27BDFFF0, // addiu sp, -0x10
			cp1,
			cp2,
			0x8D290000, // lw t1, 0(t1)
			0x1120000F, // beqz t1, +15
			0x7FBF0000, // sq ra,(sp)
			0x01202021, // move a0, t1
			po1,
			po2,
			0x01402821, // move a1, t2
			0x24840080, // addiu a0, 0x80
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 0x80),
			0x01202021, // move a0, t1
			0x01402821, // move a1, t2
			0x24840140, // addiu a0, 0x140
			0x24A50080, // addiu a1, 0x80
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 0x50),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		const Fn_SetFov Fn_setFov
		{
			fo1,
			fo2,
			0xC50C0000, // lwc1 f12, 0(t0)
			Mips::j(m_offset.Fn_unknown),
			0xE62C0124 // swc1 f12, 0x124(s1)
		};

		m_ram.write(fnSetPacketOffset(), Fn_setPacket);
		m_ram.write(fnSetFovOffset(), Fn_setFov);

		m_ram.read(m_offset.cameraPtr, &m_cameraPtr);

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
		FreecamModel::drawCameraName(FreecamModel::cameraNameMonoState);

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
		Ui::setXSpacingStr("Teleport To Camera");

		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Game Over"), &m_noGameOver);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("No Collisions"), &m_noCollisions);
		MiscModel::drawEnableButton("Teleport To Camera", "Set##TTC", &m_teleportToCamera);
		MiscModel::drawEnableButton("Unlock All", "Set##UA", &m_unlockAll);
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

			m_position.x -= cx * sy * fv;
			m_position.y -= -sx * fv;
			m_position.z -= cy * cx * fv;
			m_position.x -= (cc + ss * sx) * rv;
			m_position.y -= cx * sz * rv;
			m_position.z -= (cs * sx - sc) * rv;
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
		auto read = [&]()
		{
			if (!m_cameraPtr)
			{
				return;
			}

			float wm[4][4];
			m_ram.read(m_cameraPtr + 0x80, &wm);
			m_position = *(Vec3<float>*)wm[3];
			m_rotation.x = std::asin(-wm[2][1]);
			m_rotation.y = std::atan2(wm[2][0], wm[2][2]);
			m_rotation.z = std::atan2(wm[0][1], wm[1][1]);
			m_fov = Math::toRadians(m_ram.read<float>(m_cameraPtr + 0x1A4));
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_transposeMatrix + 0xE0, Mips::j(fnSetPacketOffset()), 0x03E00008,
			m_offset.Fn_setMatrix + 0x15C, Mips::jal(fnSetFovOffset()), Mips::jal(m_offset.Fn_unknown)
		);

		m_isEnabled ? writeCamera() : read();
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
			m_offset.Fn_updatePlayer, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0x7FB00040 },
			m_offset.Fn_updateNpc, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			m_offset.Fn_updateExtern, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FB00010 },
			m_offset.Fn_updateAction, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C040032 },
			m_offset.Fn_updateSfx, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0x3C020032 },
			m_offset.Fn_updateSfx2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0x3C060032 },
			m_offset.Fn_updateItems, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			m_offset.Fn_updateGameOver + 0x634, 0x00000000, 0x26D60001,
			m_offset.Fn_updateTimer, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FB00010 }
		);

		m_ram.writeConditional(m_isHudHidden,
			m_offset.Fn_drawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			m_offset.Fn_drawText, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0x7FB00030 }
		);

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn_padStatus + 0x178, 0xAE0C0010, 0xAE000010,
			m_offset.Fn_padStatus + 0x188, 0xAE020010, 0xAE000010,
			m_offset.Fn_padStatus + 0x1A0, 0xAE020010, 0xAE000010
		);

		m_ram.writeConditional(m_isJoystickEnabled,
			m_offset.Fn_padStatus + 0x158, 0xA2070000, 0xA2000000,
			m_offset.Fn_padStatus + 0x160, 0xA2020001, 0xA2000001,
			m_offset.Fn_padStatus + 0x164, 0xA2030002, 0xA2000002,
			m_offset.Fn_padStatus + 0x168, 0xA2040003, 0xA2000003
		);
	}

	void Loop::updateBonus()
	{
		MiscModel::teleportToCamera(&m_input, Input::TeleportToCamera, &m_teleportToCamera);

		m_ram.write(m_offset.Fn_drawFog, m_noFog ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFF30, 0x7FB000C0 });
		m_ram.write(m_offset.Fn_updateGameOver + 0x70, m_noGameOver ? 0x00009021 : 0x0062900A);
		m_ram.write(m_offset.Fn_updatePlayer + m_dep.upShift, m_noCollisions || m_teleportToCamera ? 0x00000000 : Mips::jal(m_offset.Fn_updatePlayerCollisions));

		if (m_teleportToCamera)
		{
			const auto playerPtr{ m_ram.read<u32>(m_offset.playerPtr) };

			if (playerPtr)
			{
				const auto& [px, py, pz]{ m_position };
				const auto& [rx, ry, rz]{ m_rotation };

				const auto
					sx{ std::sin(rx) },
					cx{ std::cos(rx) },
					sy{ std::sin(ry) },
					cy{ std::cos(ry) };

				static constexpr auto forwardAmount{ 1500.f };
				const Vec3<float> playerPosition
				{
					px - cx * sy * forwardAmount,
					py - -sx * forwardAmount - 1000.f,
					pz - cy * cx * forwardAmount
				};

				m_ram.write(playerPtr + 0xC0, playerPosition);
			}
			m_teleportToCamera = false;
		}

		if (m_unlockAll)
		{
			const auto current{ m_ram.read<s32>(m_offset.progression) };
			m_ram.write(m_offset.progression, current | 0xFF'DD'00'0F);
			m_ram.write(m_offset.progression + 9, u8(4)); // Ada's report
			Console::append(Console::Type::Success, "All unlocked successfully");
			m_unlockAll = false;
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
				m_rotation.z = Math::toRadians(180.f);
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
				const auto entranceMenu{ m_ram.read<s32>(m_offset.menuStruct + 0x2C) };

				if (entranceMenu != 0)
				{
					const bool isInventoryOpen{ entranceMenu == 1 };
					const bool isShopOpen{ entranceMenu == 0x10 };
					const bool isRadioOpen{ entranceMenu == 0x20 };
					const bool isFileOpen{ entranceMenu == 0x40 };
					const auto id{ m_ram.read<s8>(m_offset.menuStruct + 0x2D4) };

					// Keys Treasures | Weapons Recovery | Files
					if ((isInventoryOpen && (id == 0 || id == 1 || id == 3)) || isShopOpen || isFileOpen)
					{
						m_position = { 0.f, 0.f, 5000.f };
						m_rotation = { 0.f, 0.f, Math::toRadians(180.f) };
						m_fov = Math::toRadians(20.f);
					}
					else if (isRadioOpen)
					{
						m_position = { 0.f, 0.f, 2000.f };
						m_rotation = { 0.f, 0.f, Math::toRadians(180.f) };
						m_fov = Math::toRadians(50.f);
					}

					writeCamera();
					const auto p{ createPacket() };
					m_ram.write(m_cameraPtr + 0x80, p, 0x80);
					m_ram.write(m_cameraPtr + 0x140, *((u8*)&p + 0x80), 0x50);
					m_ram.write(m_cameraPtr + 0x1A4, Math::toDegrees(m_fov));
				}
			}
		}
	}

	void Loop::writeCamera()
	{
		m_ram.write(packetOffset(), createPacket());
		m_ram.write(fovOffset(), Math::toDegrees(m_fov));
	}

	Packet Loop::createPacket() const
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
		Packet p;

		p[0] = cc + ss * sx;
		p[1] = cx * sz;
		p[2] = cs * sx - sc;
		p[3] = 0.f;
		p[4] = sc * sx - cs;
		p[5] = cx * cz;
		p[6] = cc * sx + ss;
		p[7] = 0.f;
		p[8] = cx * sy;
		p[9] = -sx;
		p[10] = cy * cx;
		p[11] = 0.f;
		p[12] = m_position.x;
		p[13] = m_position.y;
		p[14] = m_position.z;
		p[15] = 1.f;

		p[16] = p[0];
		p[17] = p[4];
		p[18] = p[8];
		p[19] = 0.f;
		p[20] = p[1];
		p[21] = p[5];
		p[22] = p[9];
		p[23] = 0.f;
		p[24] = p[2];
		p[25] = p[6];
		p[26] = p[10];
		p[27] = 0.f;
		p[28] = p[16] * px + p[20] * py + p[24] * pz;
		p[29] = p[17] * px + p[21] * py + p[25] * pz;
		p[30] = p[18] * px + p[22] * py + p[26] * pz;
		p[31] = 1.f;

		p[32] = p[4];
		p[33] = p[5];
		p[34] = p[6];
		p[35] = 1.f;
		p[36] = p[8];
		p[37] = p[9];
		p[38] = p[10];
		p[39] = 1.f;
		p[40] = p[0];
		p[41] = p[1];
		p[42] = p[2];
		p[43] = 1.f;
		p[44] = 0.f;
		p[45] = 0.f;
		p[46] = 0.f;
		p[47] = 0.f;

		p[48] = m_position.x;
		p[49] = m_position.y;
		p[50] = m_position.z;
		p[51] = 1.f;

		return p;
	}

	u32 Loop::fnSetPacketOffset() const
	{
		return m_offset.Fn_unknown2;
	}

	u32 Loop::packetOffset() const
	{
		return fnSetPacketOffset() + sizeof(Fn_SetPacket);
	}

	u32 Loop::fnSetFovOffset() const
	{
		return packetOffset() + sizeof(Packet);
	}

	u32 Loop::fovOffset() const
	{
		return fnSetFovOffset() + sizeof(Fn_SetFov);
	}
}
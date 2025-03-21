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

#include "Game.hpp"

#include <cmath>
#include <type_traits>

namespace PS2::JadeCocoon2
{
	using Fn_SetViewMatrix = std::array<Mips_t, 7>;

	static constexpr auto viewMatrixSize{ sizeof(float[4][4]) };

	enum : u8
	{
		SIZE_SCENEMAIN = 0x28,
		SIZE_TITLE = 0x30,
		SIZE_INTERVAL = 0x28,
		SIZE_ACT = 0x98,
		SIZE_BATTLE = 0x70,
		SIZE_MENUGATE = 0x40,
		SIZE_MENUVERSUS = 0x70,
		SIZE_MENUSAVE = 0x40,
		SIZE_MENULOAD = 0x40,
		SIZE_MENUSAVE2 = 0x40,
		SIZE_MENUTEMPLE = 0x88,
		SIZE_MENUFIELD = 0x70,
		SIZE_MENUSHOP = 0x40,
		SIZE_MENUITEMBOX = 0x40,
		SIZE_MENUARENA = 0x60,
		SIZE_MENUGENUSARENA = 0x50,
		SIZE_MENUARENABOARD = 0x50,
		SIZE_MENUBBS = 0x48,
		SIZE_MENUMISSIONCOMP = 0x50,
		SIZE_MENUREFERENCE = 0x58,
		SIZE_MENURENAME = 0x58,
		SIZE_MENUNETENTRY = 0x48,
		SIZE_MENUNETTRADE = 0x48,
		SIZE_MENUHDD = 0x40,
		SIZE_ENDING = 0x28,
		SIZE_RESET = 0x28
	};

	static constexpr std::array<u8, 25> tblSizes
	{
		SIZE_TITLE, SIZE_INTERVAL, SIZE_ACT, SIZE_BATTLE,
		SIZE_MENUGATE, SIZE_MENUVERSUS, SIZE_MENUSAVE, SIZE_MENULOAD,
		SIZE_MENUSAVE2, SIZE_MENUTEMPLE, SIZE_MENUFIELD, SIZE_MENUSHOP,
		SIZE_MENUITEMBOX, SIZE_MENUARENA, SIZE_MENUGENUSARENA, SIZE_MENUARENABOARD,
		SIZE_MENUBBS, SIZE_MENUMISSIONCOMP, SIZE_MENUREFERENCE, SIZE_MENURENAME,
		SIZE_MENUNETENTRY, SIZE_MENUNETTRADE, SIZE_MENUHDD, SIZE_ENDING,
		SIZE_RESET
	};

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.sdseInstr = { 0x27BDFF10, 0x7FBF0010 };
			d.sdiInstr = 0x27BDFF60;
		}
		else if (version == Version::NtscU)
		{
			d.sdseInstr = { 0x27BDFF10, 0x7FBF0010 };
			d.sdiInstr = 0x27BDFF70;
		}
		else
		{
			d.sdseInstr = { 0x27BDFEE0, 0x7FBF0040 };
			d.sdiInstr = 0x27BDFF70;
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
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_hideHud);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				m_input.readSettings(j);
			}
			catch (const Json::Exception& e)
			{
				Console::append(Console::Type::Exception, Json::exceptionFormat, Game::name, e.what());
			}
		}

		Console::append(Console::Type::Common,
			"({}) Note: If there are many things displayed on the screen (especially in forests), the game may crash", Game::name);
	}

	Loop::~Loop()
	{
		Json::Write json;
		auto* const j{ &json };
		JSON_SET(j, m_pauseGame);
		JSON_SET(j, m_hideHud);
		JSON_SET(j, m_disableButton);
		JSON_SET(j, m_disableJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
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
		DRAW_GAME_WINDOWS(drawFreecam(), m_input.draw(), drawSettings(), ImGui::TextUnformatted("None"));
		PS2_PCSX2_DRAW_PNACH_BEHAVIOR_WINDOW(Game);
		PS2_DEBUG_DRAW_WINDOW(Game);
	}

	void Loop::update()
	{
		const Fn_SetViewMatrix Fn_setViewMatrix
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x7FBF0000, // sq ra,(sp)
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		m_ram.write(m_offset.Fn_std___BCD___BCD, Fn_setViewMatrix);

		const auto id{ m_ram.read<s32>(m_offset.gM2MainEnv + 8) };
		m_cameraPtr = 0;

		if (id < tblSizes.size() && id >= 0)
		{
			auto tblPtr{ m_ram.read<u32>(m_offset.gM2MainEnv + 0x24) };
			const auto tblSize{ tblPtr == m_offset.gGMEP_SceneEventTable ? SIZE_SCENEMAIN : tblSizes[id] };

			tblPtr += id << 2;
			m_ram.read(tblPtr, &tblPtr);
			std::vector<u32> tblPtrs(tblSize / sizeof(u32));
			m_ram.read(tblPtr, tblPtrs.data(), tblSize);

			for (s32 i{}; i < tblSize / 8; ++i)
			{
				const auto ptr{ tblPtrs[i * 2] };
				if (ptr == 0)
				{
					break;
				}
				m_ram.read(ptr, &tblPtr);
				tblPtr += 0x10;

				if (m_ram.read<u32>(tblPtr + 0x8) == m_offset.Fn_EvM2Camera_Control)
				{
					m_ram.read(tblPtr + 0x14, &m_cameraPtr);
					break;
				}
			}
		}

		updateFreecam();
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
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Hide Hud"), &m_hideHud);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Disable Joystick"), &m_disableJoystick);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
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
		auto read = [&]()
		{
			float vm[3][4];
			m_ram.read(m_cameraPtr + 0x120, &m_position);
			m_ram.read(m_cameraPtr + 0x250, &vm);
			m_fov = Math::toRadians(m_ram.read<float>(m_cameraPtr + 0x34) * 2.f);
			m_rotation.x = std::asin(-vm[1][2]);
			m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
			m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
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

			const std::array<float, 2> packet
			{
				Math::toDegrees(m_fov / 2.f),
				0.15f // Near
			};

			m_ram.write(viewMatrixOffset(), vm, viewMatrixSize);
			m_ram.write(m_cameraPtr + 0x34, packet);
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_m2MapCamera + 0x34, 0x00000000, 0xE4800034, // Fov
			m_offset.Fn_m2Camera_Init + 0xA4, 0x00000000, 0xE6000034, // Fov
			m_offset.Fn_m2Camera_ExeFcurve + 0x2AC, 0x00000000, 0xE4600034, // Fov
			m_offset.Fn_m2Camera_ExeFcurve + 0x36C, 0x00000000, 0xE4600034, // End Battle Fov
			m_offset.Fn_m2Camera_InitFixedPos + 0x10C, 0x00000000, 0xE6000034 // Fov
		);

		if (m_cameraPtr)
		{
			m_isEnabled ? write() : read();
		}

		const std::array<Mips_t, 3> sceVu0CameraMatrixCall
		{
			0x24470040,	// addiu a3, v0, 0x40
			Mips::jal(m_offset.Fn_sceVu0CameraMatrix),
			0x00000000	// nop
		};

		const auto [vmo1, vmo2] { Mips::li32(Mips::Register::a1, viewMatrixOffset()) };
		const std::array<Mips_t, 3> setViewMatrixCall
		{
			vmo1,
			Mips::jal(m_offset.Fn_std___BCD___BCD),
			vmo2
		};

		m_ram.write(m_offset.Fn__XVIVIEW__SetMatrix + 0x1F4, m_isEnabled ? setViewMatrixCall : sceVu0CameraMatrixCall);
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

		m_ram.write(m_offset.newPause, m_isGamePaused ? 1 : 0);
		m_ram.writeConditional(m_isHudHidden,
			m_offset.Fn_m2Sprite_Disp, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0x7FBF0000 },
			m_offset.Fn_m2Fix_SjisDispString, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			m_offset.Fn_m2Fix_SjisDispValue, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			m_offset.Fn_m2Fix_SjisDispStringEx, Mips::jrRaNop(), m_dep.sdseInstr,
			m_offset.Fn_m2Window_DispWindow, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			m_offset.Fn_M2AutoMap_Draw, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFD10, 0x7FBF0090 },
			m_offset.Fn_m2Selector_DispIcon, Mips::jrRaNop(), std::array<Mips_t, 2>{ m_dep.sdiInstr, 0x7FBF0030 }
		);

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn__gnkPadman_StandardStatus + 0x30, 0x8FA40040, 0x00002021,
			m_offset.Fn__gnkPadman_StandardStatus + 0x44, 0x8C840000, 0x00002021
		);

		m_ram.write(m_offset.Fn__gnkPadman_StandardStatus + 0x188, m_isJoystickEnabled ? 0x90640000 : 0x2404007F);
	}

	void Loop::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			FreecamModel::resetSpeed(m_resetMovementSpeed, m_resetRotationSpeed, m_resetFovSpeed);
			m_controls.resetVelocity();

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
				m_ram.write(m_cameraPtr + 0x38, 1.f); // Near
			}
		}
	}

	u32 Loop::viewMatrixOffset() const
	{
		return m_offset.Fn_std___BCD___BCD + sizeof(Fn_SetViewMatrix);
	}
}
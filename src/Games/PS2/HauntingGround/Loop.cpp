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

#include <array>
#include <cmath>
#include <type_traits>

namespace PS2::HauntingGround
{
	using Fn_SetViewMatrixIfMainCamera = std::array<Mips_t, 15>;

	static constexpr auto viewMatrixSize{ sizeof(float[4][4]) };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.uiShift = 0xD94;
			d.umugShift = 0x258;
			d.umogShift = 0x5CC;
			d.uagShift = 0x240;
			d.utsReg = Mips::Register::a2;
		}
		else
		{
			d.uiShift = 0xD8C;
			d.umugShift = 0x230;
			d.umogShift = 0x5AC;
			d.uagShift = 0x218;
			d.utsReg = Mips::Register::a3;
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
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableLJoystick);
				JSON_GET(j, m_disableRJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_noShade);
				JSON_GET(j, m_noBlur);
				JSON_GET(j, m_noTextures);
				JSON_GET(j, m_noSubtitles);
				JSON_GET(j, m_noCutsceneEffect);
				JSON_GET(j, m_noCutsceneBlackBar);
				JSON_GET(j, m_runByDefault);
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
		JSON_SET(j, m_disableLJoystick);
		JSON_SET(j, m_disableRJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_noShade);
		JSON_SET(j, m_noBlur);
		JSON_SET(j, m_noTextures);
		JSON_SET(j, m_noSubtitles);
		JSON_SET(j, m_noCutsceneEffect);
		JSON_SET(j, m_noCutsceneBlackBar);
		JSON_SET(j, m_runByDefault);
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
		// MainMatrixCallSetMatrixReturnOffset
		const auto [mmcsmro1, mmcsmro2]{ Mips::li32(Mips::Register::v1, m_offset.Fn_setMainMatrix + 0x28) };
		const auto [vmo1, vmo2]{ Mips::li32(Mips::Register::a1, viewMatrixOffset()) };

		const Fn_SetViewMatrixIfMainCamera Fn_svmifmc
		{
			0x27BDFFF0, // addiu sp, -0x10
			0x8FA200F0, // lw v0, 0xF0(sp)
			mmcsmro1,
			mmcsmro2,
			0x10430003, // beq v0, v1, +3
			0x7FBF0000, // sq ra,(sp)
			Mips::j(m_offset.Fn_sceVu0CameraMatrix),
			0x27BD0010, // addiu sp, 0x10
			vmo1,
			vmo2,
			Mips::jal(m_offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, viewMatrixSize),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		m_ram.write(m_offset.Fn_std__default_new_handler, Fn_svmifmc);

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
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Loop::drawOthers()
	{
		MiscModel::drawMiscPauseGame(&m_isGamePaused, !m_isEnabled);
		MiscModel::drawControllerButtonLRJoystick(&m_isButtonEnabled, &m_isLJoystickEnabled, &m_isRJoystickEnabled, !m_isEnabled);
	}

	void Loop::drawSettings()
	{
		Ui::setXSpacingStr("Reset Movement Speed");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Reset Z Rotation"), &m_resetZRotation);
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Disable Left Joystick"), &m_disableLJoystick);
		Ui::checkbox(Ui::lol("Disable Right Joystick"), &m_disableRJoystick);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
	}

	void Loop::drawBonus()
	{
		Ui::setXSpacingStr("No Cutscene Black Bar");

		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Shade"), &m_noShade);
		Ui::checkbox(Ui::lol("No Blur"), &m_noBlur);
		Ui::checkbox(Ui::lol("No Textures"), &m_noTextures);
		Ui::checkbox(Ui::lol("No Subtitles"), &m_noSubtitles);
		Ui::checkbox(Ui::lol("No Cutscene Effect"), &m_noCutsceneEffect);
		Ui::hoveredTooltip("Remove on screen cutscene effect");
		Ui::checkbox(Ui::lol("No Cutscene Black Bar"), &m_noCutsceneBlackBar);
		Ui::checkbox(Ui::lol("Run By Default"), &m_runByDefault);
		Ui::hoveredTooltip("Fiona run by default and can walk by holding the running button");

		Ui::separatorText("Cheats");
		MiscModel::drawEnableButton("Secret Room", "Unlock All", &m_unlockAllSecretRoom);
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
			m_ram.read(m_cameraPtr + 0x60, &m_position);
			m_ram.read(m_cameraPtr + 0xD0, &vm);
			m_ram.read(m_cameraPtr + 0x14, &m_fov);
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

			m_ram.write(viewMatrixOffset(), vm, viewMatrixSize);
			m_ram.write(m_cameraPtr + 0x14, m_fov);
			m_ram.write(m_cameraPtr + 0xC, 0.25f); // Near
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_setMatrix + 0x148, Mips::jal(m_offset.Fn_std__default_new_handler), Mips::jal(m_offset.Fn_sceVu0CameraMatrix),
			m_offset.Fn_initMapProjection + 0x60, 0x00000000, 0x0320F809, // Near
			m_offset.Fn_initMapProjection + 0xB0, 0x00000000, 0x0320F809, // Fov
			m_offset.Fn_cameraTransition + 0x164, 0x00000000, 0x0320F809, // Fov
			m_offset.Fn_setFov + 4, 0x00000000, 0xE48C0014,
			m_offset.Fn_updateModelViewer + 0x3C4, 0x00000000, 0x0320F809
		);

		if (m_cameraPtr)
		{
			m_isEnabled ? write() : read();
		}
	}

	void Loop::updateOthers()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(&m_input, Input::LJoystick, &m_isLJoystickEnabled);
			MiscModel::toggle(&m_input, Input::RJoystick, &m_isRJoystickEnabled);
		}

		m_ram.writeConditional(m_isGamePaused,
			m_offset.Fn_updateIngame + m_dep.uiShift, 0x1000004F, 0x3C0200F7,
			m_offset.Fn_isParticleShouldntBeAnimated + 0x34, 0x24020001, 0x00000000,
			m_offset.Fn_updatePanic + 0x54, 0x1000000B, 0x1440000B
		);

		const bool isAdxRunning{ m_ram.read<u8>(m_offset.adxFlags) == 1 };
		const auto sofdecVideoStructPtr{ m_ram.read<u32>(m_offset.sofdecVideoStructPtr) };
		const bool isInCutscene{ sofdecVideoStructPtr != 0 };
		const bool isCutscenePaused{ sofdecVideoStructPtr ? (m_ram.read<s32>(sofdecVideoStructPtr + 0x50) == 1 ? true : false) : false };

		if (isInCutscene && !isCutscenePaused)
		{
			if (m_isGamePaused && isAdxRunning)
			{
				m_ram.write(m_offset.adxFlags, u8(0)); // Pause
				m_ram.write(m_offset.Fn_isSofdecVideoRunning + 0x30, 0x00001021);
			}
			else if (!m_isGamePaused && !isAdxRunning)
			{
				m_ram.write(m_offset.adxFlags, u8(1)); // Resume
				m_ram.write(m_offset.Fn_isSofdecVideoRunning + 0x30, 0x00000000);
			}
		}

		m_ram.writeConditional(m_isButtonEnabled,
			m_offset.Fn_padStatus + 0xE4, 0xAE270004, 0xAE200004, // Up
			m_offset.Fn_padStatus + 0x11C, 0xAE260004, 0xAE200004, // Right
			m_offset.Fn_padStatus + 0x14C, 0xAE230004, 0xAE200004, // Left
			m_offset.Fn_padStatus + 0x188, 0xAE220004, 0xAE200004, // Down
			m_offset.Fn_padStatus + 0x1C4, 0xAE240004, 0xAE200004, // L1
			m_offset.Fn_padStatus + 0x1EC, 0xAE230004, 0xAE200004, // R1
			m_offset.Fn_padStatus + 0x36C, 0xA6650014, 0xA6600014, // Button
			m_offset.Fn_padStatus + 0x394, 0xA665001C, 0xA660001C, // Button Temp
			m_offset.Fn_padCrossTriangle + 0x58, 0xAC680004, 0xAC600004, // Cross
			m_offset.Fn_padCrossTriangle + 0x7C, 0xAC670004, 0xAC600004 // Triangle
		);

		const auto jalSceVu0ScaleVectorXYZ{ Mips::jal(m_offset.Fn_sceVu0ScaleVectorXYZ) };
		m_ram.write(m_offset.Fn_padStatus + 0x5A0, m_isLJoystickEnabled ? jalSceVu0ScaleVectorXYZ : 0x7C800000);
		m_ram.write(m_offset.Fn_padStatus + 0x718, m_isRJoystickEnabled ? jalSceVu0ScaleVectorXYZ : 0x7C800000);
	}

	void Loop::updateBonus()
	{
		m_ram.write(m_offset.Fn_drawFog + 0x7C, m_noFog ? 0x100001D6 : 0x104001D6);
		m_ram.write(m_offset.Fn_drawShade + 0x48, m_noShade ? 0x100004B2 : 0x104004B2);
		m_ram.write(m_offset.Fn_drawBlur + 0x4C, m_noBlur ? 0x10000804 : 0x12200804);
		m_ram.writeConditional(m_noTextures,
			m_offset.Fn_drawEntity + 0xA4, 0x10000031, 0x12420031,
			m_offset.Fn_drawEntity + 0x1C8, 0x1000000A, 0x1040000A, // Cutscene
			m_offset.Fn_drawStaticMap + 0x19C, 0x10000007, 0x10820007,
			m_offset.Fn_drawDynamicMap + 0x148, 0x10000007, 0x10820007
		);
		m_ram.write(m_offset.Fn_drawSubtitles + 0xC, m_noSubtitles ? 0x10000006 : 0x10A00006);
		m_ram.write(m_offset.Fn_cutsceneFrame + 0xA4, m_noCutsceneEffect ? 0x1000001B : 0x1060001B);
		m_ram.write(m_offset.Fn_blackBarCutscene + 0xC, m_noCutsceneBlackBar ? 0x00000000 : 0x10600003);
		m_ram.write(m_offset.Fn_fionaMovementControls + 0x554, m_runByDefault ? 0x146001AA : 0x106001AA);

		if (m_unlockAllSecretRoom)
		{
			m_ram.write(m_offset.Fn_updateTitleScreen + 0x64, Mips::li(m_dep.utsReg, 1)); // Cursor
			m_ram.write(m_offset.Fn_drawTitleScreen + 0x190, 0x24020001); // Text
			m_ram.write(m_offset.Fn_drawTitleScreen + 0x1CC, 0x24020001); // Cursor Light
			m_ram.write(m_offset.Fn_updateMusicGallery + m_dep.umugShift, 0x00000000); // Title
			m_ram.write(m_offset.Fn_musicGalleryPlayMusic + 0x88, 0x1000008C);
			m_ram.write(m_offset.Fn_updateMovieGallery + 0x10C, 0x100000CA); // Play Movie
			m_ram.write(m_offset.Fn_updateMovieGallery + m_dep.umogShift, 0x00000000); // Title
			m_ram.write(m_offset.Fn_updateMovieGallery + m_dep.umogShift + 0x288, 0x1000006C); // Preview
			m_ram.write(m_offset.Fn_updateArtGallery + m_dep.uagShift, 0x00000000); // Title
			m_ram.write(m_offset.Fn_artGalleryPlayImage + 0xC4, 0x100000B8);
			m_ram.write(m_offset.Fn_isCharacterGalleryUnlocked + 0x120, 0x24020001);
			m_ram.write(m_offset.Fn_initCostumeMenu + 0xF0, 0x00000000); // Texas Cowgirl
			m_ram.write(m_offset.Fn_initCostumeMenu + 0x104, 0x00000000); // Frog
			m_ram.write(m_offset.Fn_initCostumeMenu + 0x1EC, 0x00000000); // Stuffed Toy
			m_ram.write(m_offset.Fn_initCostumeMenu + 0x1FC, 0x10000004); // Stuffed Toy

			Console::append(Console::Type::Success, "All unlocked successfully");
			m_unlockAllSecretRoom = false;
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
			m_isButtonEnabled = true;
			m_isLJoystickEnabled = true;
			m_isRJoystickEnabled = true;

			if (m_cameraPtr)
			{
				m_ram.write(m_cameraPtr + 0xC, 1.0f); // Near
			}
		}
	}

	u32 Loop::viewMatrixOffset() const
	{
		return m_offset.Fn_std__default_new_handler + sizeof(Fn_SetViewMatrixIfMainCamera);
	}
}
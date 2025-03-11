#include "Loop.hpp"

#include "Common/PS1/Debug.hpp"
#include "Common/PS1/libgte.hpp"
#include "Common/PS1/PS1.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Console.hpp"
#include "Common/FreecamModel.hpp"
#include "Common/Json.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>
#include <cmath>
#include <type_traits>

namespace PS1::DinoCrisis
{
	static constexpr auto posMultiplyScalar{ 16.f };
	static constexpr auto fovMin{ 0.20f }, fovMax{ 2.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::NtscJ)
		{
			d.psShift = 0x114;
		}
		else
		{
			d.psShift = 0x108;
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
		const auto jsonRead{ Json::read(PS1::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			try
			{
				const auto& j{ jsonRead.value() };
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noCutsceneBlackBar);
				JSON_GET(j, m_noGameOverFade);
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
		JSON_SET(j, m_pauseGame);
		JSON_SET(j, m_disableButton);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noCutsceneBlackBar);
		JSON_SET(j, m_noGameOverFade);
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
		CameraModel::drawFov(&m_fov, !m_isEnabled, fovMin, fovMax);
	}

	void Loop::drawOthers()
	{
		MiscModel::drawMiscPauseGame(&m_isGamePaused, !m_isEnabled);
		MiscModel::drawControllerButton(&m_isButtonEnabled, !m_isEnabled);
	}

	void Loop::drawSettings()
	{
		Ui::setXSpacingStr("Reset Movement Speed");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Pause Game"), &m_pauseGame);
		Ui::checkbox(Ui::lol("Disable Button"), &m_disableButton);
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &m_resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &m_resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &m_resetFovSpeed);
	}

	void Loop::drawBonus()
	{
		Ui::setXSpacingStr("No Cutscene Black Bar");

		Ui::checkbox(Ui::lol("No Cutscene Black Bar"), &m_noCutsceneBlackBar);
		Ui::checkbox(Ui::lol("No Game Over Fade"), &m_noGameOverFade);
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
			libgte::MATRIX view;
			m_ram.read(m_offset.mainCamera, &view);
			m_ram.read(m_offset.mainCamera + 0x30, &m_position);

			m_rotation.x = std::asin(fixedToFloat(view.m[2][1]));
			m_rotation.y = std::atan2(fixedToFloat(-view.m[2][0]), fixedToFloat(view.m[2][2]));
			m_rotation.z = std::atan2(fixedToFloat(-view.m[0][1]), fixedToFloat(view.m[1][1]));

			const auto
				sx{ std::sin(m_rotation.x) },
				cx{ std::cos(m_rotation.x) },
				sy{ std::sin(m_rotation.y) },
				cy{ std::cos(m_rotation.y) };

			m_position.x -= -cx * sy * view.t[2];
			m_position.y -= sx * view.t[2];
			m_position.z -= cx * cy * view.t[2];
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
			view.t[0] = view.t[1] = view.t[2] = 0;

			const auto fov{ static_cast<s16>(floatToFixed(m_fov)) };

			for (std::size_t i{}; i < 6; ++i)
			{
				auto* const v{ (s16*)view.m + i };
				*v = *v * fov >> 12;
			}

			auto writeCamera = [&](u32 ptr)
			{
				m_ram.write(ptr, view);
				m_ram.write(ptr + 0x30, m_position);
				m_ram.write(ptr + 0x3C, s16(0)); // Shadow Forward
			};

			writeCamera(m_offset.mainCamera);
			writeCamera(m_offset.dinosaurCamera);
		};

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_updateMainCamera + 0x60, std::array<Mips_t, 3>{ 0, 0, 0 }, std::array<Mips_t, 3>{ 0xAC430014, 0xAC440018, 0xAC45001C }, // Eye
			m_offset.Fn_setCameraPosition, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xAFB70054 },
			m_offset.Fn_updateCamera + 0xF8, 0x00000000, Mips::jal(m_offset.Fn_RotMatrix),
			m_offset.Fn_setNextFrameCamera + 0x12C, 0x00000000, 0xA622003C, // Shadow Forward
			m_offset.Fn_moveNextCamera, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 }, // Cutscene Position
			m_offset.Fn_updateLinearCutsceneCamera + 0x74, 0x00000000, 0xA623003C, // Shadow Forward
			m_offset.Fn_updateLinearCutsceneCamera + 0xE0, 0x00000000, 0xA6220020, // X
			m_offset.Fn_updateLinearCutsceneCamera + 0xF4, 0x00000000, 0xA6220022, // Y
			m_offset.Fn_updateLinearCutsceneCamera + 0x108, 0x00000000, 0xA6220024, // Z
			m_offset.Fn_updateGameOver + 0x64, 0x00000000, 0xA604003C, // Shadow Forward
			m_offset.Fn_dinosaurEntranceCameraTransition + 0x138, 0x00000000, 0xA6020030, // X
			m_offset.Fn_dinosaurEntranceCameraTransition + 0x144, 0x00000000, 0xA6020032, // Y
			m_offset.Fn_dinosaurEntranceCameraTransition + 0x150, 0x00000000, 0xA6020034 // Z
		);

		m_isEnabled ? write() : read();
	}

	void Loop::updateOthers()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(&m_input, Input::PauseGame, &m_isGamePaused);
			MiscModel::toggle(&m_input, Input::Button, &m_isButtonEnabled);
		}

		m_ram.writeConditional(m_isGamePaused,
			m_offset.Fn_updateMap, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 },
			m_offset.Fn_updateInteractable, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x3C021F80 },
			m_offset.Fn_updateCharacters, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 },
			m_offset.Fn_updateItems, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C031F80 },
			m_offset.Fn_updateSfx, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 }
		);

		const auto offset{ m_offset.Fn_padStatus + m_dep.psShift };

		m_ram.writeConditional(m_isButtonEnabled,
			offset, 0xA4A70000, 0xA4A00000,
			offset + 0x10, 0xA4A20002, 0xA4A00002
		);
	}

	void Loop::updateBonus()
	{
		m_ram.write(m_offset.Fn_drawEffects + 0x228, m_noCutsceneBlackBar ? 0x1000006E : 0x1440006E);
		const auto drawGameOverFadeJump{ 0x80000000 + m_offset.Fn_drawFade + (m_noGameOverFade ? 0x34 : 0x4C) };
		m_ram.write(m_offset.drawFadeTbl + 0xC, drawGameOverFadeJump);
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
			if (m_disableButton)
			{
				m_isButtonEnabled = false;
			}
		}
		else
		{
			m_isGamePaused = false;
			m_isButtonEnabled = true;
			m_fov = 1.f;

			if (m_ram.read<u8>(m_offset.mainCamera + 0x70) != 0) // Mode
			{
				auto packet{ m_ram.read<std::array<s16, 7>>(m_offset.mainCamera + 0x30) };
				const auto target{ m_ram.read<libgte::SVECTOR>(m_offset.mainCamera + 0x40) };
				const auto position{ m_ram.read<libgte::SVECTOR>(m_offset.mainCamera + 0x20) };

				const auto
					x{ std::abs(position.vx - target.vx) },
					y{ std::abs(position.vy - target.vy) },
					z{ std::abs(position.vz - target.vz) };

				packet[0] = target.vx;
				packet[1] = target.vy;
				packet[2] = target.vz;
				packet[6] = static_cast<s16>(std::sqrtf(static_cast<float>(x * x + y * y + z * z)));

				m_ram.write(m_offset.mainCamera + 0x1C, packet[6]);
				m_ram.write(m_offset.mainCamera + 0x30, packet);
			}
		}
	}
}
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

namespace PS2::Sly1
{
	static constexpr auto posMultiplyScalar{ 32.f };

	static inline auto createVersionDependency(s32 version)
	{
		VersionDependency d;

		if (version == Version::Pal)
		{
			d.worldLevelIdShift = 0xA0;
			d.hideoutPtrShift = 0x570;
			d.dlbInstr = { 0x27BDFFB0, 0x3C014259 };
			d.dlInstr = { 0x27BDFF80, 0x7FB30040 };
			d.dcInstr = { 0x27BDFEB0, 0x7FB20120 };
			d.dpInstr = { 0x27BDFDC0, 0x7FB30170 };
			d.dbInstr = { 0x27BDF820, 0x7FB20730 };
			d.dtInstr = { 0x27BDFEA0, 0x7FB10110 };
			d.dnInstr = { 0x27BDFEA0, 0x7FB20120 };
			d.dt2Instr = { 0x27BDFEB0, 0x7FB10110 };
		}
		else if (version == Version::NtscU)
		{
			d.worldLevelIdShift = 0x20;
			d.hideoutPtrShift = 0x260;
			d.dlbInstr = { 0x27BDFFB0, 0x3C014284 };
			d.dlInstr = { 0x27BDFF80, 0x7FB30040 };
			d.dcInstr = { 0x27BDFED0, 0x7FB10100 };
			d.dpInstr = { 0x27BDFDD0, 0x7FB20150 };
			d.dbInstr = { 0x27BDFB50, 0x7FB20400 };
			d.dtInstr = { 0x27BDFEB0, 0x7FB10100 };
			d.dnInstr = { 0x27BDFEC0, 0x7FB10100 };
			d.dt2Instr = { 0x27BDFF60, 0x7FB10050 };
		}
		else
		{
			d.worldLevelIdShift = 0x40;
			d.hideoutPtrShift = 0x570;
			d.dlbInstr = { 0x27BDFFB0, 0x3C014284 };
			d.dlInstr = { 0x27BDFE60, 0x7FB30140 };
			d.dcInstr = { 0x27BDFEB0, 0x7FB20120 };
			d.dpInstr = { 0x27BDFDC0, 0x7FB30170 };
			d.dbInstr = { 0x27BDF820, 0x7FB20730 };
			d.dtInstr = { 0x27BDFEA0, 0x7FB10110 };
			d.dnInstr = { 0x27BDFEA0, 0x7FB20120 };
			d.dt2Instr = { 0x27BDFEB0, 0x7FB10110 };
		}

		d.levelFlagsShift = d.worldLevelIdShift + 8;
		d.levelSize = d.worldLevelIdShift + 0xC;

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
				JSON_GET(j, m_resetXRotation);
				JSON_GET(j, m_pauseGame);
				JSON_GET(j, m_hideHud);
				JSON_GET(j, m_disableButton);
				JSON_GET(j, m_disableJoystick);
				JSON_GET(j, m_resetMovementSpeed);
				JSON_GET(j, m_resetRotationSpeed);
				JSON_GET(j, m_resetFovSpeed);
				JSON_GET(j, m_noMotionBlur);
				JSON_GET(j, m_invulnerable);
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
		JSON_SET(j, m_resetXRotation);
		JSON_SET(j, m_pauseGame);
		JSON_SET(j, m_hideHud);
		JSON_SET(j, m_disableButton);
		JSON_SET(j, m_disableJoystick);
		JSON_SET(j, m_resetMovementSpeed);
		JSON_SET(j, m_resetRotationSpeed);
		JSON_SET(j, m_resetFovSpeed);
		JSON_SET(j, m_noMotionBlur);
		JSON_SET(j, m_invulnerable);
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
		m_ram.write(m_offset.Fn_antiPiracy1 + 0xB4, 0x00001021);
		m_ram.write(m_offset.Fn_antiPiracy2 + 0xC8, 0x00001021);
		m_ram.write(m_offset.Fn_antiPiracy3 + 0x134, 0x00001021);
		m_ram.write(m_offset.Fn_antiPiracy4 + 0xD8, 0x10000008);

		m_ram.read(m_offset.g_pcm, &m_cameraPtr);

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
		Ui::checkbox(Ui::lol("Reset X Rotation"), &m_resetXRotation);
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

		Ui::checkbox(Ui::lol("No Motion Blur"), &m_noMotionBlur);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("Invulnerable"), &m_invulnerable);
		MiscModel::drawEnableButton("Keys", "Unlock All##Keys", &m_unlockAllKeys);
		MiscModel::drawEnableButton("Power-ups", "Unlock All##Power-ups", &m_unlockAllPowerUps);
		MiscModel::drawEnableButton("Teleport To Camera", "Set##TTC", &m_teleportToCamera);
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
				cc{ cx * cz },
				cs{ cx * sz },
				sc{ sx * cz },
				ss{ sx * sz };

			const auto
				fv{ m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward) * posMultiplyScalar },
				rv{ m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft) * posMultiplyScalar },
				uv{ m_controls.upVelocity(Input::MoveUp, Input::MoveDown) * posMultiplyScalar };

			m_position.x += cy * cz * fv;
			m_position.y += -cy * sz * fv;
			m_position.z += sy * fv;
			m_position.x -= (cs + sy * sc) * rv;
			m_position.y -= (cc - sy * ss) * rv;
			m_position.z -= -cy * sx * rv;
			m_position.x += (ss - sy * cc) * uv;
			m_position.y += (sc + sy * cs) * uv;
			m_position.z += cx * cy * uv;

			CameraModel::rotateRoll(&m_rotation.x, m_controls.rollVelocity(Input::RotateXPos, Input::RotateXNeg));
			CameraModel::rotatePitch(&m_rotation.y, m_fov, m_controls.pitchVelocity(Input::RotateYPos, Input::RotateYNeg));
			CameraModel::rotateYaw(&m_rotation.z, m_fov, m_controls.yawVelocity(Input::RotateZPos, Input::RotateZNeg));
			CameraModel::increaseFov(&m_fov, m_controls.fovVelocity(Input::FovPos, Input::FovNeg));
		}
	}

	void Loop::updateCamera()
	{
		auto read = [&]()
		{
			float rm[3][4];
			m_ram.read(m_cameraPtr + 0x40, &m_position);
			m_ram.read(m_cameraPtr + 0x80, &rm);
			m_ram.read(m_cameraPtr + 0x1C4, &m_fov);
			m_rotation.x = std::atan2(-rm[1][2], rm[2][2]);
			m_rotation.y = std::asin(rm[0][2]);
			m_rotation.z = std::atan2(-rm[0][1], rm[0][0]);
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
				cc{ cx * cz },
				cs{ cx * sz },
				sc{ sx * cz },
				ss{ sx * sz };

			float rm[3][4];
			rm[0][0] = cy * cz;
			rm[0][1] = -cy * sz;
			rm[0][2] = sy;
			rm[1][0] = cs + sy * sc;
			rm[1][1] = cc - sy * ss;
			rm[1][2] = -cy * sx;
			rm[2][0] = ss - sy * cc;
			rm[2][1] = sc + sy * cs;
			rm[2][2] = cx * cy;
			rm[0][3] = rm[1][3] = rm[2][3] = 0.f;

			const auto farClip{ m_ram.read<float>(m_cameraPtr + 0x1E8) };
			writeProjectionMatrix(m_fov, 20.f, farClip);
			m_ram.write(m_cameraPtr + 0x40, m_position);
			m_ram.write(m_cameraPtr + 0x80, rm);
		};

		const auto jalUpdateCmMat4{ Mips::jal(m_offset.Fn_UpdateCmMat4__FP2CM) };

		m_ram.writeConditional(m_isEnabled,
			m_offset.Fn_SetCm__FP2CMP6VECTORT1ff + 0x50, 0x00000000, 0x7E020040, // Pos
			m_offset.Fn_SetCm__FP2CMP6VECTORT1ff + 0xC8, 0x00000000, Mips::jal(m_offset.Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3), // Rot
			m_offset.Fn_RecalcCmFrustrum__FP2CM + 0x44, 0x00000000, 0xE60001F4,
			m_offset.Fn_RecalcCmFrustrum__FP2CM + 0x50, 0x00000000, 0xE60C01F0,
			m_offset.Fn_RecalcCmFrustrum__FP2CM + 0xB0, 0x00000000, Mips::jal(m_offset.Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4),

			m_offset.Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff + 0x3BC,
				std::array<Mips_t, 5>{ 0x00000000, 0x00000000, 0x00000000, jalUpdateCmMat4, 0x00000000 },
				std::array<Mips_t, 5>{ 0x7E060040, 0x7E020080, 0x7E030090, jalUpdateCmMat4, 0x7E0500A0 }, // Pos Rot

			m_offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0x74, 0x00000000, 0x450103E5, // Force recalc if game paused
			m_offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xEA8, 0x00000000, 0xFAA10040, // Pos
			m_offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF80, 0x00000000, 0xFAA70080, // Rot
			m_offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF88, 0x00000000, 0x7EA20090, // Rot
			m_offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF94, 0x00000000, 0x7EA300A0, // Rot
			m_offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x18, 0x00000000, 0x7E020040, // Pos
			m_offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x28, 0x00000000, 0x7E020080, // Rot
			m_offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x30, 0x00000000, 0x7E030090, // Rot
			m_offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x38, 0x00000000, 0x7E0200A0, // Rot
			m_offset.Fn_UpdateCmMat4__FP2CM + 0x48, 0x00000000, 0x4500000A // Boss frustrum
		);

		if (m_cameraPtr)
		{
			m_isEnabled ? write() : read();
		};
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

		m_ram.write(m_offset.Fn_MarkClockTick__FP5CLOCK + 0x98, m_isGamePaused ? 0x00001021 : 0x8E020000);

		m_ram.writeConditional(m_isHudHidden,
			m_offset.Fn_DrawLetterbox__FP9LETTERBOX, Mips::jrRaNop(), m_dep.dlbInstr,
			m_offset.Fn_DrawLogo__FP4LOGO, Mips::jrRaNop(), m_dep.dlInstr,
			m_offset.Fn_DrawAttract__FP7ATTRACT, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C020026, 0x3C014040 },
			m_offset.Fn_DrawCtr__FP3CTR, Mips::jrRaNop(), m_dep.dcInstr,
			m_offset.Fn_DrawPrompt__FP6PROMPT, Mips::jrRaNop(), m_dep.dpInstr,
			m_offset.Fn_DrawBinoc__FP5BINOC, Mips::jrRaNop(), m_dep.dbInstr,
			m_offset.Fn_DrawTv__FP2TV, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C020026 },
			m_offset.Fn_DrawTitle__FP5TITLE, Mips::jrRaNop(), m_dep.dtInstr,
			m_offset.Fn_DrawWmc__FP3WMC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFE80, 0x7FB300F0 },
			m_offset.Fn_DrawBossctr__FP7BOSSCTR, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF00, 0x7FB500B0 },
			m_offset.Fn_DrawNote__FP4NOTE, Mips::jrRaNop(), m_dep.dnInstr,
			m_offset.Fn_DrawTimer__FP5TIMER, Mips::jrRaNop(), m_dep.dt2Instr
		);

		m_ram.write(m_offset.Fn_FReadJoy__FP3JOY + 0x60, m_isButtonEnabled ? 0xA62200A8 : 0xA62000A8);

		m_ram.writeConditional(m_isJoystickEnabled,
			m_offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1B4, 0xE5220000, 0xAD200000,
			m_offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1E4, 0xE4E00000, 0xACE00000,
			m_offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1EC, 0xE5010000, 0xAD000000
		);
	}

	void Loop::updateBonus()
	{
		MiscModel::teleportToCamera(&m_input, Input::TeleportToCamera, &m_teleportToCamera);

		m_ram.write(m_offset.Fn_BlendPrevFrame__Fv + 0x15C, m_noMotionBlur ? 0x00003021 : 0x24060030);
		m_ram.write(m_offset.g_fInvulnerable, m_invulnerable || m_teleportToCamera ? 1 : 0);

		enum : s32
		{
			LEVEL_PROGRESS_AVAILABLE = 1 << 0,
			LEVEL_PROGRESS_KEY = 1 << 1,
			LEVEL_PROGRESS_VAULT = 1 << 2,
			LEVEL_PROGRESS_SPRINT = 1 << 3
		};

		auto getLevelInfos = [&]()
		{
			const auto bufferSize{ 46 * m_dep.levelSize };
			Buffer levelInfos(bufferSize);
			m_ram.read(m_offset.levelInfo, levelInfos.data(), bufferSize);
			return levelInfos;
		};

		auto worldFlagNumber = [&](u8* ptr, s32 world, s32 flag)
		{
			s32 number{};
			for (s32 i{}; i < 46; ++i)
			{
				const auto levelIndex{ m_dep.levelSize * i };
				if (*(s32*)(ptr + levelIndex + m_dep.worldLevelIdShift) >> 8 == world)
				{
					if (*(s32*)(ptr + levelIndex + m_dep.levelFlagsShift) & flag)
					{
						++number;
					}
				}
			}
			return number;
		};

		auto enableLevelsFlags = [&](u8* ptr, s32 flags)
		{
			auto allowedWorldLevelFlags = [&](s32 world, s32 level)
			{
				const s32 worldLevel{ (world << 8) + level };
				for (s32 i{}; i < 46; ++i)
				{
					const auto levelIndex{ m_dep.levelSize * i };
					if (*(s32*)(ptr + levelIndex + m_dep.worldLevelIdShift) == worldLevel)
					{
						return *(s32*)(ptr + levelIndex + m_dep.levelFlagsShift);
					}
				}
				return 0;
			};

			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ m_offset.g_gsCur + (world * 0x44C) };
				for (s32 level{}; level < 9; ++level)
				{
					const auto offsetProgress{ worldOffset + 0x10 + (0x78 * level) };
					m_ram.write(offsetProgress, m_ram.read<s32>(offsetProgress) | (allowedWorldLevelFlags(world, level) & flags));
				}
			}
		};

		auto enableLevelsAvailability = [&]()
		{
			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ m_offset.g_gsCur + (world * 0x44C) };
				for (s32 level{}; level < 9; ++level)
				{
					const auto offsetProgress{ worldOffset + 0x10 + (0x78 * level) };
					m_ram.write(offsetProgress, m_ram.read<s32>(offsetProgress) | LEVEL_PROGRESS_AVAILABLE);
				}
			}
		};

		if (m_unlockAllKeys)
		{
			auto levelInfos{ getLevelInfos() };
			auto* const ptr{ levelInfos.data() };

			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ m_offset.g_gsCur + (world * 0x44C) };
				m_ram.write(worldOffset + 0x448, worldFlagNumber(ptr, world, LEVEL_PROGRESS_KEY)); // World Keys
				m_ram.write(worldOffset + 0x458, 1); // World open
			}

			enableLevelsAvailability();
			enableLevelsFlags(ptr, LEVEL_PROGRESS_KEY);

			const auto hideoutPtr{ m_ram.read<u32>(m_offset.g_prompt + m_dep.hideoutPtrShift) };
			if (hideoutPtr)
			{
				m_ram.write(hideoutPtr + 0x2FC, 5);
			}

			Console::append(Console::Type::Success, "Keys unlocked successfully");
			m_unlockAllKeys = false;
		}

		if (m_unlockAllPowerUps)
		{
			auto levelInfos{ getLevelInfos() };
			auto* const ptr{ levelInfos.data() };

			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ m_offset.g_gsCur + (world * 0x44C) };
				m_ram.write(worldOffset + 0x44C, worldFlagNumber(ptr, world, LEVEL_PROGRESS_VAULT)); // World Vaults
			}

			m_ram.write(m_offset.g_gsCur + 0x19F0, -1); // Power-ups
			enableLevelsFlags(ptr, LEVEL_PROGRESS_VAULT);

			Console::append(Console::Type::Success, "Power-ups unlocked successfully");
			m_unlockAllPowerUps = false;
		}

		auto flyingJt = [&](bool flag)
		{
			// Should be improved by disabling collisions and positions rollback
			m_ram.write(m_offset.Fn_FInvulnerableJt__FP2JT3ZPK + 0xCC, flag ? 0x00000000 : 0x10400018); // Death Barriers
		};

		if (m_teleportToCamera)
		{
			const auto jtPtr{ m_ram.read<u32>(m_offset.g_pjt) };

			if (jtPtr)
			{
				const auto& [px, py, pz]{ m_position };
				const auto& [rx, ry, rz]{ m_rotation };

				const auto
					sy{ std::sin(ry) },
					cy{ std::cos(ry) },
					sz{ std::sin(rz) },
					cz{ std::cos(rz) };

				static constexpr auto forwardAmount{ 500.f };
				const Vec3<float> jtPosition
				{
					px + cy * cz * forwardAmount,
					py + -cy * sz * forwardAmount,
					pz + sy * forwardAmount
				};

				flyingJt(true);
				m_ram.write(jtPtr + 0x100, jtPosition);
			}
			m_teleportToCamera = false;
		}
		else
		{
			flyingJt(false);
		}
	}

	void Loop::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			FreecamModel::resetSpeed(m_resetMovementSpeed, m_resetRotationSpeed, m_resetFovSpeed);
			m_controls.resetVelocity();

			if (m_resetXRotation)
			{
				m_rotation.x = 0.f;
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
				const auto fov{ m_ram.read<float>(m_cameraPtr + 0x1C4) };
				const auto [nearClip, farClip]{ m_ram.readPacket<float, float>(m_cameraPtr + 0x1E4) };
				writeProjectionMatrix(fov, nearClip, farClip);
			}
		}
	}

	void Loop::writeProjectionMatrix(float fov, float nearClip, float farClip) const
	{
		const auto
			aspectRatio{ m_ram.read<float>(m_cameraPtr + 0x1E0) },
			tanHalfFov{ std::tan(fov / 2.f) };

		float proj[4][4]{};
		proj[0][0] = 640.f / (tanHalfFov * aspectRatio * 4096.f);
		proj[1][1] = (m_version == Version::Pal ? 256.f : 224.f) / (tanHalfFov * 4096.f);
		proj[2][2] = (nearClip + farClip) / (nearClip - farClip);
		proj[2][3] = 1.f;
		proj[3][2] = nearClip * (1.f - (nearClip + farClip) / (nearClip - farClip));

		const std::array<float, 2> frustrum
		{
			tanHalfFov * aspectRatio, tanHalfFov
		};

		m_ram.write(m_cameraPtr + 0xC0, proj);
		m_ram.write(m_cameraPtr + 0x1F0, frustrum);
	}
}
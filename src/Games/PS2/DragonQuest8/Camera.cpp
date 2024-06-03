#include "Camera.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "Game.hpp"

#include <array>
#include <cmath>

namespace PS2::DragonQuest8
{
	// Note: the rotation matrix is inverted by the game

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	u32 Camera::ptr() const
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto cameraPtr{ ram.read<u32>(offset.cameraPtr) };
		if (cameraPtr)
		{
			const auto shift{ m_game->version() == Version::Pal ? 0x3864 : 0x3860 };
			const auto gameState{ ram.read<s32>(offset.gameState) };
			return ram.read<u32>(cameraPtr + (((gameState << 4) - gameState) << 2) + shift + 0x38);
		}
		return 0;
	}

	void Camera::draw()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation, !m_isEnabled);
		CameraModel::drawFovDegrees(&m_fov, !m_isEnabled);
	}

	void Camera::update()
	{
		enableGameCamera(!m_isEnabled);
		m_isEnabled ? write() : read();
	}

	void Camera::enable(bool enable)
	{
		if (!enable && m_isEnabled)
		{
			const auto cameraPtr{ ptr() };
			if (cameraPtr)
			{
				const auto& ram{ m_game->ram() };
				const auto& offset{ m_game->offset() };
		
				ram.write(offset.fov, Math::toRadians(ram.read<float>(cameraPtr)));
				const auto nearShift{ m_game->version() == Version::Pal ? 0x20 : 0x1C };
				ram.write(offset.projectionStruct + nearShift, 5.f);
			}
		}
		else if (m_game->settings()->resetZRotation)
		{
			m_rotation.z = 0.f;
		}
		m_isEnabled = enable;
	}

	void Camera::moveForward(float amount)
	{
		const auto
			sx{ -std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ -std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) };

		m_position.x -= -cx * sy * amount;
		m_position.y -= sx * amount;
		m_position.z -= cx * cy * amount;
	}

	void Camera::moveRight(float amount)
	{
		const auto
			sx{ -std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ -std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) },
			sz{ -std::sin(m_rotation.z) },
			cz{ std::cos(m_rotation.z) },
			ss{ sz * sx };

		m_position.x += (cz * cy - ss * sy) * amount;
		m_position.y += -cx * sz * amount;
		m_position.z += (cz * sy + ss * cy) * amount;
	}

	void Camera::moveUp(float amount)
	{
		const auto
			sx{ -std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ -std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) },
			sz{ -std::sin(m_rotation.z) },
			cz{ std::cos(m_rotation.z) },
			cs{ cz * sx };

		m_position.x += (sz * cy + cs * sy) * amount;
		m_position.y += cz * cx * amount;
		m_position.z += (sz * sy - cs * cy) * amount;
	}

	void Camera::rotateX(float amount)
	{
		CameraModel::rotatePitch(&m_rotation.x, m_fov, amount);
	}

	void Camera::rotateY(float amount)
	{
		CameraModel::rotateYaw(&m_rotation.y, m_fov, amount);
	}

	void Camera::rotateZ(float amount)
	{
		CameraModel::rotateRoll(&m_rotation.z, amount);
	}

	void Camera::increaseFov(float amount)
	{
		CameraModel::increaseFov(&m_fov, amount);
	}

	void Camera::read()
	{
		const auto cameraPtr{ ptr() };

		if (!cameraPtr)
		{
			return;
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		// For some reasons, position and rotation of the minimap are completely broken
		if (m_game->state() == State::Alchemy_Minimap && !PS2::isValidMemoryRange(ram.read<u32>(offset.alchemyPtr)))
		{
			m_position = {};
			m_rotation = {};
			return;
		}

		float rot[3][4];
		ram.read(offset.fov, &m_fov);
		ram.read(cameraPtr + 0x40, &m_position);
		ram.read(cameraPtr + 0x10, &rot);
		m_rotation.x = -std::asin(rot[2][1]);
		m_rotation.y = -std::atan2(-rot[2][0], rot[2][2]);
		m_rotation.z = -std::atan2(-rot[0][1], rot[1][1]);
	}

	void Camera::write()
	{
		const auto cameraPtr{ ptr() };

		if (!cameraPtr)
		{
			return;
		}

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

		float rot[3][3];
		rot[0][0] = cc - ss * sx;
		rot[0][1] = -cx * sz;
		rot[0][2] = cs + sc * sx;
		rot[1][0] = sc + cs * sx;
		rot[1][1] = cz * cx;
		rot[1][2] = ss - cc * sx;
		rot[2][0] = -cx * sy;
		rot[2][1] = sx;
		rot[2][2] = cx * cy;

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const bool isInCutscene{ ram.read<s32>(offset.cutsceneState) == 2 };

		if (m_game->state() == State::Common && !isInCutscene)
		{
			auto eye{ m_position };
			static constexpr auto z{ 9.999996185f };
			eye.x += -rot[2][0] * z;
			eye.y += -rot[2][1] * z;
			eye.z += -rot[2][2] * z;

			const std::array<float, 23> packet
			{
				rot[0][0], rot[0][1], rot[0][2], 0.f,
				rot[1][0], rot[1][1], rot[1][2], 0.f,
				rot[2][0], rot[2][1], rot[2][2], 0.f,
				m_position.x, m_position.y, m_position.z, 1.f,
				eye.x, eye.y, eye.z, 1.f,
				m_position.x, m_position.y, m_position.z
			};

			ram.write(cameraPtr + 0x10, packet);
		}
		else
		{
			const std::array<float, 15> packet
			{
				rot[0][0], rot[0][1], rot[0][2], 0.f,
				rot[1][0], rot[1][1], rot[1][2], 0.f,
				rot[2][0], rot[2][1], rot[2][2], 0.f,
				m_position.x, m_position.y, m_position.z
			};

			ram.write(cameraPtr + 0x10, packet);
		}

		ram.write(offset.fov, m_fov);
		const auto nearShift{ m_game->version() == Version::Pal ? 0x20 : 0x1C };
		ram.write(offset.projectionStruct + nearShift, 1.f);
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto cameraPtr{ ptr() };
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto jalAddVector{ Mips::jal(offset.Fn_mgAddVector__FPfPf) };

		if (cameraPtr)
		{
			const auto writePositionCallback{ ram.read<u32>(cameraPtr + 0xC0) + 0x40 };
			if (writePositionCallback > m_game->pnachInfo().textSectionEnd)
			{
				ram.write(writePositionCallback, enable ? offset.Fn_CreatePose__7CCameraFv : offset.Fn_nullsub1);
			}
		}

		Mips_t
			spmInstr,
			spmInstr2;

		if (m_game->version() == Version::Pal)
		{
			spmInstr = 0xE4800014;
			spmInstr2 = 0xE48C0014;
		}
		else
		{
			spmInstr = 0xE4800010;
			spmInstr2 = 0xE48C0010;
			// Workaround to fix a skybox bug when the pitch is high or low, fixed in pal version
			ram.write(offset.Fn_drawSkybox + 0x8C, std::fabs(m_rotation.x) > 0.6981316f ? 0x10000096 : 0x45000096);
		}

		ram.writeConditional(enable,
			offset.Fn_Step__14CCameraControlFf + 0x94, jalAddVector, 0x00000000,
			offset.Fn_Step__14CCameraControlFf + 0xD8, jalAddVector, 0x00000000,
			offset.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo + 0x1C, spmInstr, 0x00000000, // Fov
			offset.Fn_SetPrimitiveMatrix__13mgCRenderInfoFR18mgRenderMatrixInfo + 0x1C0, spmInstr2, 0x00000000, // Fov
			offset.Fn_Roll__9mgCCameraFf, std::array<Mips_t, 2>{ 0x27BDFF50, 0xFFBF0020 }, Mips::jrRaNop(),
			offset.Fn_SetPos__7CCameraFPf + 0x10, 0x7C830060, 0x00000000,
			offset.Fn_SetPos__7CCameraFPf + 0x30, 0x7C830050, 0x00000000, // Cutscene
			offset.Fn_drawCompass, std::array<Mips_t, 2>{ 0x27BDFC10, 0xFFBF0090 }, Mips::jrRaNop()
		);
	}
}
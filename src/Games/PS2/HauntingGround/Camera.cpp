#include "Camera.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"

#include "CustomCode.hpp"
#include "Game.hpp"

#include <cmath>

namespace PS2::HauntingGround
{
	Camera::Camera(Game* game)
		: m_game(game)
	{
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
			m_game->ram().write(m_game->ram().read<u32>(m_game->offset().cameraPtr) + 0xC, 1.0f); // Near
		}
		m_isEnabled = enable;
	}

	void Camera::moveForward(float amount)
	{
		const auto
			sx{ std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) };

		m_position.x += cx * sy * amount;
		m_position.y += -sx * amount;
		m_position.z += cy * cx * amount;
	}

	void Camera::moveRight(float amount)
	{
		const auto
			sx{ std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) },
			sz{ std::sin(m_rotation.z) },
			cz{ std::cos(m_rotation.z) },
			ss{ sx * sz };

		m_position.x += (cy * cz + ss * sy) * amount;
		m_position.y += cx * sz * amount;
		m_position.z += (ss * cy - sy * cz) * amount;
	}

	void Camera::moveUp(float amount)
	{
		const auto
			sx{ std::sin(m_rotation.x) },
			cx{ std::cos(m_rotation.x) },
			sy{ std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) },
			sz{ std::sin(m_rotation.z) },
			cz{ std::cos(m_rotation.z) },
			cs{ cz * sx };

		m_position.x -= (cs * sy - cy * sz) * amount;
		m_position.y -= cx * cz * amount;
		m_position.z -= (cs * cy + sy * sz) * amount;
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
		const auto& ram{ m_game->ram() };
		const auto cameraPtr{ ram.read<u32>(m_game->offset().cameraPtr) };

		if (!cameraPtr)
		{
			return;
		}

		float vm[3][4];
		ram.read(cameraPtr + 0x60, &m_position);
		ram.read(cameraPtr + 0xD0, &vm);
		ram.read(cameraPtr + 0x14, &m_fov);
		m_rotation.x = std::asin(-vm[1][2]);
		m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
		m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
	}

	void Camera::write()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto cameraPtr{ ram.read<u32>(offset.cameraPtr) };

		if (!cameraPtr)
		{
			return;
		}

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

		if (CustomCode::isApplied(*m_game))
		{
			ram.write(CustomCode::viewMatrixOffset(*m_game), vm, CustomCode::viewMatrixSize);
			ram.write(offset.Fn_setMatrix + 0x148, Mips::jal(offset.Fn_std__default_new_handler));
			ram.write(cameraPtr + 0x14, m_fov);
			ram.write(cameraPtr + 0xC, 0.25f); // Near
		}
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		if (enable)
		{
			ram.write(offset.Fn_setMatrix + 0x148, Mips::jal(offset.Fn_sceVu0CameraMatrix));
		}

		ram.writeConditional(enable,
			offset.Fn_initMapProjection + 0x60, 0x0320F809, 0x00000000, // Near
			offset.Fn_initMapProjection + 0xB0, 0x0320F809, 0x00000000, // Fov
			offset.Fn_cameraTransition + 0x164, 0x0320F809, 0x00000000, // Fov
			offset.Fn_setFov + 4, 0xE48C0014, 0x00000000,
			offset.Fn_updateModelViewer + 0x3C4, 0x0320F809, 0x00000000
		);
	}
}
#include "Camera.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "Game.hpp"

#include <array>
#include <cmath>

namespace PS2::DBZBudokai3
{
	static constexpr auto duPosMultiplyScalar{ 32.f };

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	void Camera::draw()
	{
		auto posSpeed{ g_settings.dragFloatSpeed };

		if (m_game->state() == State::DragonUniverse)
		{
			posSpeed *= duPosMultiplyScalar;
		}

		CameraModel::drawPosition(&m_position, posSpeed, !m_isEnabled);
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
		if (enable && m_game->settings()->resetZRotation)
		{
			m_rotation.z = 0.f;
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

		if (m_game->state() == State::DragonUniverse)
		{
			amount *= duPosMultiplyScalar;
		}

		m_position.x -= cx * sy * amount;
		m_position.y -= -sx * amount;
		m_position.z -= cy * cx * amount;
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

		if (m_game->state() == State::DragonUniverse)
		{
			amount *= duPosMultiplyScalar;
		}

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

		if (m_game->state() == State::DragonUniverse)
		{
			amount *= duPosMultiplyScalar;
		}

		m_position.x += (cs * sy - cy * sz) * amount;
		m_position.y += cx * cz * amount;
		m_position.z += (cs * cy + sy * sz) * amount;
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
		const auto state{ m_game->state() };

		if (state == State::None)
		{
			return;
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		float vm[3][4];

		if (state == State::Battle)
		{
			const auto cameraPtr{ ram.read<u32>(offset.battleCameraPtr) };
			const auto cameraPtrState{ ram.read<u32>(cameraPtr + 0x264) };

			ram.read(cameraPtrState, &m_position);
			ram.read(cameraPtrState + 0x34, &m_fov);
			ram.read(cameraPtr + 0x60, &vm);
		}
		else
		{
			ram.read(offset.duPosition, &m_position);
			ram.read(offset.duPosition + 0x50, &vm);
			m_fov = 448.f / 512.f / 2.f;
		}

		m_rotation.x = std::asin(-vm[1][2]);
		m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
		m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
	}

	void Camera::write()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto state{ m_game->state() };

		if (state == State::Battle)
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

			ram.write(offset.battleCommonCamera - 0x40, packet); // Cutscene
			ram.write(offset.battleCommonCamera, packet);
		}
		else if (state == State::DragonUniverse)
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

			ram.write(offset.duPosition + 0x50, vm);
			ram.write(offset.globalFov, 448.f / (m_fov * 2.f));
		}
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto version{ m_game->version() };
		const auto state{ m_game->state() };

		if (enable)
		{
			ram.write(offset.globalFov, 512.f);
		}

		ram.write(offset.Fn_drawModel, state == State::DragonUniverse && !enable ?
			Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 });

		u32 bscpShift,
			ducShift;

		Mips_t 
			bucInstr,
			bucInstr2;

		if (version == Version::PalCollector || version == Version::NtscJ)
		{
			bscpShift = 0xBC;
			ducShift = 0x228;
			bucInstr = 0x14830018;
			bucInstr2 = 0x10000018;
		}
		else if (version == Version::NtscU_GH)
		{
			bscpShift = 0xB8;
			ducShift = 0x220;
			bucInstr = 0x1083000B;
			bucInstr2 = 0x00000000;
		}
		else
		{
			bscpShift = 0xB4;
			ducShift = 0x228;
			bucInstr = 0x14830018;
			bucInstr2 = 0x10000018;
		}

		ram.writeConditional(state == State::None || enable,
			offset.Fn_battleSetCameraPosition + bscpShift, 0x0040F809, 0x00000000,
			offset.Fn_battleSetQuaternion, std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0020 }, Mips::jrRaNop(),
			offset.Fn_battleUpdateCamera + 0x2C, bucInstr, bucInstr2, // Cutscene
			offset.Fn_duUpdateCamera + ducShift,
				std::array<Mips_t, 4>{ 0xF85C0000, 0xF85D0010, 0xF85E0020, 0xF85F0030 },
				std::array<Mips_t, 4>{ 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
		);
	}
}
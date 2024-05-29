#include "Camera.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Math.hpp"
#include "Common/Mips.hpp"

#include "Game.hpp"

#include <cmath>

namespace PS2::ResidentEvil4
{
	static constexpr auto posMultiplyScalar{ 32.f };

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	CustomCode::Packet Camera::createPacket()
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
		CustomCode::Packet p;

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

	void Camera::draw()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed * posMultiplyScalar, !m_isEnabled);
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
		if (!enable)
		{
			const auto& ram{ m_game->ram() };
			const auto& offset{ m_game->offset() };
			const auto cameraPtr{ ram.read<u32>(offset.cameraPtr) };

			if (cameraPtr)
			{
				const auto entranceMenu{ ram.read<s32>(offset.menuStruct + 0x2C) };

				if (entranceMenu != 0)
				{
					const bool isInventoryOpen{ entranceMenu == 1 };
					const bool isShopOpen{ entranceMenu == 0x10 };
					const bool isRadioOpen{ entranceMenu == 0x20 };
					const bool isFileOpen{ entranceMenu == 0x40 };
					const auto id{ ram.read<s8>(offset.menuStruct + 0x2D4) };

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

					write();
					const auto p{ createPacket() };
					ram.write(cameraPtr + 0x80, p, 0x80);
					ram.write(cameraPtr + 0x140, *((u8*)&p + 0x80), 0x50);
					ram.write(cameraPtr + 0x1A4, Math::toDegrees(m_fov));
				}
			}
		}
		else if (m_game->settings()->resetZRotation)
		{
			m_rotation.z = Math::toRadians(180.f);
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

		amount *= posMultiplyScalar;

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

		amount *= posMultiplyScalar;

		m_position.x -= (cy * cz + ss * sy) * amount;
		m_position.y -= cx * sz * amount;
		m_position.z -= (ss * cy - sy * cz) * amount;
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

		amount *= posMultiplyScalar;

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

		float wm[4][4];
		ram.read(cameraPtr + 0x80, &wm);

		m_position = *(Vec3<float>*)wm[3];
		m_rotation.x = std::asin(-wm[2][1]);
		m_rotation.y = std::atan2(wm[2][0], wm[2][2]);
		m_rotation.z = std::atan2(wm[0][1], wm[1][1]);
		m_fov = Math::toRadians(ram.read<float>(cameraPtr + 0x1A4));
	}

	void Camera::write()
	{
		const auto& ram{ m_game->ram() };
		ram.write(CustomCode::packetOffset(*m_game), createPacket());
		ram.write(CustomCode::fovOffset(*m_game), Math::toDegrees(m_fov));
	}

	void Camera::enableGameCamera(bool enable)
	{
		if (CustomCode::isApplied(*m_game))
		{
			const auto& offset{ m_game->offset() };

			m_game->ram().writeConditional(enable,
				offset.Fn_transposeMatrix + 0xE0, 0x03E00008, Mips::j(CustomCode::fnSetPacketOffset(*m_game)),
				offset.Fn_setMatrix + 0x15C, Mips::jal(offset.Fn_unknown), Mips::jal(CustomCode::fnSetFovOffset(*m_game))
			);
		}
	}
}
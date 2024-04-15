#include "Camera.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "CustomCode.hpp"
#include "Game.hpp"

#include <array>
#include <cmath>

namespace PS2::DBZTenkaichi3
{
	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	std::pair<u32, u32> Camera::vmPosPtr(s32 state) const
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		auto get = [](u32 vm, u32 vmShift, u32 pos, u32 posShift) -> std::pair<u32, u32>
		{
			if (vm && pos)
			{
				return { vm + vmShift, pos + posShift };
			}
			return { 0, 0 };
		};

		if (state == State::Battle)
		{
			const auto player1Ptr{ ram.read<u32>(offset.battlePlayerPtr) + 4 };
			return get(ram.read<u32>(offset.battlePlayerPtr + 0xBC), 0x40, ram.read<u32>(player1Ptr), 0x430);
		}
		else if (state == State::BattleCutscene || state == State::ShenronCutscene)
		{
			const auto ptr{ ram.read<u32>(offset.battlePlayerPtr + 0xB8) };
			return get(ptr, 0x40, ptr, 0x2A0);
		}
		else if (state == State::DragonHistory || state == State::ShenronWish)
		{
			const auto ptr{ ram.read<u32>(offset.battlePlayerPtr + 0xB8) };
			return get(ptr, 0x40, ptr, 0x2D0);
		}
		else if (state == State::ViewModel)
		{
			return get(ram.read<u32>(offset.battlePlayerPtr + 0xBC), 0x40, ram.read<u32>(offset.battlePlayerPtr + 0xC8), 0);
		}

		return { 0, 0 };
	}

	void Camera::draw()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_euler.pitch, &m_euler.yaw, &m_euler.roll, !m_isEnabled);
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
			m_euler.roll = 0.f;
		}

		m_isEnabled = enable;
	}

	void Camera::moveForward(float amount)
	{
		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) };

		m_position.x += cx * sy * amount;
		m_position.y += -sx * amount;
		m_position.z += cy * cx * amount;
	}

	void Camera::moveRight(float amount)
	{
		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) },
			sz{ std::sin(m_euler.roll) },
			cz{ std::cos(m_euler.roll) },
			ss{ sx * sz };

		m_position.x += (cy * cz + ss * sy) * amount;
		m_position.y += cx * sz * amount;
		m_position.z += (ss * cy - sy * cz) * amount;
	}

	void Camera::moveUp(float amount)
	{
		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) },
			sz{ std::sin(m_euler.roll) },
			cz{ std::cos(m_euler.roll) },
			cs{ cz * sx };

		m_position.x -= (cs * sy - cy * sz) * amount;
		m_position.y -= cx * cz * amount;
		m_position.z -= (cs * cy + sy * sz) * amount;
	}

	void Camera::rotateX(float amount)
	{
		CameraModel::rotatePitch(&m_euler.pitch, amount);
	}

	void Camera::rotateY(float amount)
	{
		CameraModel::rotateYaw(&m_euler.yaw, amount);
	}

	void Camera::rotateZ(float amount)
	{
		CameraModel::rotateRoll(&m_euler.roll, amount);
	}

	void Camera::read()
	{
		const auto state{ m_game->state() };
		const auto [vmPtr, posPtr]{ vmPosPtr(state) };

		if (!vmPtr || !posPtr)
		{
			return;
		}

		const auto& ram{ m_game->ram() };
		
		float vm[4][4];
		ram.read(vmPtr, &vm);
		ram.read(posPtr, &m_position);

		m_euler.roll = std::atan2(vm[1][0], vm[1][1]);
		m_euler.pitch = std::asin(-vm[1][2]);
		m_euler.yaw = std::atan2(vm[0][2], vm[2][2]);

		if (state == State::BattleCutscene)
		{
			const std::array<float, 3> vec{ m_position.x, m_position.y, m_position.z };
			m_position = {};
			ram.read(posPtr - 0x40, &vm);

			for (s32 i{}; i < 3; ++i)
			{
				m_position.x += vm[i][0] * vec[i];
				m_position.y += vm[i][1] * vec[i];
				m_position.z += vm[i][2] * vec[i];
			}

			m_position.x += vm[3][0];
			m_position.y += vm[3][1];
			m_position.z += vm[3][2];
		}
	}

	void Camera::write()
	{
		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) },
			sz{ std::sin(m_euler.roll) },
			cz{ std::cos(m_euler.roll) },
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

		m_game->ram().write(CustomCode::viewMatrixOffset(*m_game), vm, CustomCode::viewMatrixSize);
	}

	void Camera::enableGameCamera(bool enable)
	{
		if (CustomCode::isApplied(*m_game))
		{
			const auto& offset{ m_game->offset() };
			const auto 
				jal_setViewMatrix{ Mips::jal(offset.Fn_setViewMatrix) },
				jal_ccSetViewMatrix{ Mips::jal(offset.Fn_unknown) },
				jal_sceVu0InversMatrix{ Mips::jal(offset.Fn_sceVu0InversMatrix) };

			const auto cucShift{ m_game->version() == Version::Pal ? 0x4AC : 0x4B4 };

			m_game->ram().writeConditional(m_game->state() == State::None || enable,
				offset.Fn_battleSetPlayerCamera + 0x28, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x26050040 }, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x26040040 },
				offset.Fn_viewModelUpdateCamera + 0x280, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x0200202D }, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x00A02021 },
				// Story - Shenron
				offset.Fn_cutsceneUpdateCamera + 0x140, std::array<Mips_t, 2>{ jal_setViewMatrix, 0x24850040 }, std::array<Mips_t, 2>{ jal_ccSetViewMatrix, 0x24840040 },
				// Cutscene
				offset.Fn_cutsceneUpdateCamera + cucShift, jal_sceVu0InversMatrix, jal_ccSetViewMatrix,
				offset.Fn_cutsceneUpdateCamera + cucShift + 0xD4, jal_sceVu0InversMatrix, 0x00000000
			);
		}
	}
}
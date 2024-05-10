#include "Camera.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"

#include "CustomCode.hpp"
#include "Game.hpp"

#include <array>
#include <cmath>

namespace PS2::JadeCocoon2
{
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

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	u32 Camera::ptr() const
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		const auto id{ ram.read<s32>(offset.gM2MainEnv + 8) };

		if (id > tblSizes.size() || id < 0)
		{
			return 0;
		}

		auto tblPtr{ ram.read<u32>(offset.gM2MainEnv + 0x24) };
		const auto tblSize{ tblPtr == offset.gGMEP_SceneEventTable ? SIZE_SCENEMAIN : tblSizes[id] };

		tblPtr += id << 2;
		ram.read(tblPtr, &tblPtr);
		std::vector<u32> tblPtrs(tblSize / sizeof(u32));
		ram.read(tblPtr, tblPtrs.data(), tblSize);

		for (s32 i{}; i < tblSize / 8; ++i)
		{
			const auto ptr{ tblPtrs[i * 2] };
			if (ptr == 0)
			{
				break;
			}
			ram.read(ptr, &tblPtr);
			tblPtr += 0x10;

			if (ram.read<u32>(tblPtr + 0x8) == offset.Fn_EvM2Camera_Control)
			{
				const auto cameraPtr{ ram.read<u32>(tblPtr + 0x14) };
				return PS2::isValidMemoryRange(cameraPtr) ? cameraPtr : 0;
			}
		}

		return 0;
	}

	void Camera::draw()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed, !m_isEnabled);
		CameraModel::drawRotation(&m_rotation.x, &m_rotation.y, &m_rotation.z, !m_isEnabled);
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
				m_game->ram().write(cameraPtr + 0x38, 1.0f); // Near
			}
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
		const auto cameraPtr{ ptr() };

		if (!cameraPtr)
		{
			return;
		}

		const auto& ram{ m_game->ram() };

		float vm[3][4];
		ram.read(cameraPtr + 0x120, &m_position);
		ram.read(cameraPtr + 0x250, &vm);
		m_fov = Math::toRadians(ram.read<float>(cameraPtr + 0x34) * 2.f);
		m_rotation.x = std::asin(-vm[1][2]);
		m_rotation.y = std::atan2(vm[0][2], vm[2][2]);
		m_rotation.z = std::atan2(vm[1][0], vm[1][1]);
	}

	void Camera::write()
	{
		const auto cameraPtr{ ptr() };

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

		const auto& offset{ m_game->offset() };
		const auto __BCDOffset{ offset.Fn_std___BCD___BCD };
		const auto li32_vmOffset{ Mips::li32(Mips::Register::a1, CustomCode::viewMatrixOffset(*m_game)) };
		const std::array<Mips_t, 3> setViewMatrixCall
		{
			li32_vmOffset[0],
			Mips::jal(__BCDOffset),
			li32_vmOffset[1]
		};

		if (CustomCode::isApplied(*m_game))
		{
			const std::array<float, 2> packet
			{
				Math::toDegrees(m_fov / 2.f),
				0.15f // Near
			};

			const auto& ram{ m_game->ram() };

			ram.write(CustomCode::viewMatrixOffset(*m_game), vm, CustomCode::viewMatrixSize);
			ram.write(offset.Fn__XVIVIEW__SetMatrix + 0x1F4, setViewMatrixCall);
			ram.write(cameraPtr + 0x34, packet);
		}
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		if (enable)
		{
			const std::array<Mips_t, 3> sceVu0CameraMatrixCall
			{
				0x24470040,	// addiu a3, v0, 0x40
				Mips::jal(offset.Fn_sceVu0CameraMatrix),
				0x00000000  // nop
			};
			ram.write(offset.Fn__XVIVIEW__SetMatrix + 0x1F4, sceVu0CameraMatrixCall);
		}

		ram.writeConditional(enable,
			offset.Fn_m2MapCamera + 0x34, 0xE4800034, 0x00000000, // Fov
			offset.Fn_m2Camera_Init + 0xA4, 0xE6000034, 0x00000000, // Fov
			offset.Fn_m2Camera_ExeFcurve + 0x2AC, 0xE4600034, 0x00000000, // Fov
			offset.Fn_m2Camera_ExeFcurve + 0x36C, 0xE4600034, 0x00000000, // End Battle Fov
			offset.Fn_m2Camera_InitFixedPos + 0x10C, 0xE6000034, 0x00000000 // Fov
		);
	}
}
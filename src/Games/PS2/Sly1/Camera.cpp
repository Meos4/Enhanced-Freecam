#include "Camera.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "Game.hpp"

#include <array>
#include <cmath>

namespace PS2::Sly1
{
	static constexpr auto posMultiplyScalar{ 32.f };

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	void Camera::writeProjectionMatrix(u32 ptrCamera, float fov, float nearClip, float farClip) const
	{
		const auto& ram{ m_game->ram() };

		const auto
			aspectRatio{ ram.read<float>(ptrCamera + 0x1E0) },
			tanHalfFov{ std::tan(fov / 2.f) };

		float proj[4][4]{};
		proj[0][0] = 640.f / (tanHalfFov * aspectRatio * 4096.f);
		proj[1][1] = (m_game->version() == Version::Pal ? 256.f : 224.f) / (tanHalfFov * 4096.f);
		proj[2][2] = (nearClip + farClip) / (nearClip - farClip);
		proj[2][3] = 1.f;
		proj[3][2] = nearClip * (1.f - (nearClip + farClip) / (nearClip - farClip));

		const std::array<float, 2> frustrum
		{
			tanHalfFov * aspectRatio, tanHalfFov
		};

		ram.write(ptrCamera + 0xC0, proj);
		ram.write(ptrCamera + 0x1F0, frustrum);
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
		const auto& ram{ m_game->ram() };
		const auto cameraPtr{ ram.read<u32>(m_game->offset().g_pcm) };

		if (cameraPtr && m_isEnabled)
		{
			const auto fov{ ram.read<float>(cameraPtr + 0x1C4) };
			const auto packet{ ram.read<std::array<float, 2>>(cameraPtr + 0x1E4) };
			writeProjectionMatrix(cameraPtr, fov, packet[0], packet[1]);
		}
		else if (enable && m_game->settings()->resetXRotation)
		{
			m_rotation.x = 0.f; 
		}

		m_isEnabled = enable;
	}

	void Camera::moveForward(float amount)
	{
		const auto
			sy{ std::sin(m_rotation.y) },
			cy{ std::cos(m_rotation.y) },
			sz{ std::sin(m_rotation.z) },
			cz{ std::cos(m_rotation.z) };

		amount *= posMultiplyScalar;

		m_position.x += cy * cz * amount;
		m_position.y += -cy * sz * amount;
		m_position.z += sy * amount;
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
			ss{ sx * sy };

		amount *= posMultiplyScalar;

		m_position.x -= (cx * sz + ss * cz) * amount;
		m_position.y -= (cx * cz - ss * sz) * amount;
		m_position.z -= -cy * sx * amount;
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
			cs{ cx * sy };

		amount *= posMultiplyScalar;

		m_position.x += (sx * sz - cs * cz) * amount;
		m_position.y += (cz * sx + cs * sz) * amount;
		m_position.z += cx * cy * amount;
	}

	void Camera::rotateX(float amount)
	{
		CameraModel::rotateRoll(&m_rotation.x, amount);
	}

	void Camera::rotateY(float amount)
	{
		CameraModel::rotatePitch(&m_rotation.y, m_fov, amount);
	}

	void Camera::rotateZ(float amount)
	{
		CameraModel::rotateYaw(&m_rotation.z, m_fov, amount);
	}

	void Camera::increaseFov(float amount)
	{
		CameraModel::increaseFov(&m_fov, amount);
	}

	void Camera::read()
	{
		const auto& ram{ m_game->ram() };
		const auto cameraPtr{ ram.read<u32>(m_game->offset().g_pcm) };

		if (!cameraPtr)
		{
			return;
		};

		float rot[3][4];
		ram.read(cameraPtr + 0x40, &m_position);
		ram.read(cameraPtr + 0x80, &rot);
		ram.read(cameraPtr + 0x1C4, &m_fov);
		m_rotation.x = std::atan2(-rot[1][2], rot[2][2]);
		m_rotation.y = std::asin(rot[0][2]);
		m_rotation.z = std::atan2(-rot[0][1], rot[0][0]);
	}

	void Camera::write()
	{
		const auto& ram{ m_game->ram() };
		const auto cameraPtr{ ram.read<u32>(m_game->offset().g_pcm) };

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
			cc{ cx * cz },
			cs{ cx * sz },
			sc{ sx * cz },
			ss{ sx * sz };

		float rot[3][4];
		rot[0][0] = cy * cz;
		rot[0][1] = -cy * sz;
		rot[0][2] = sy;
		rot[1][0] = cs + sy * sc;
		rot[1][1] = cc - sy * ss;
		rot[1][2] = -cy * sx;
		rot[2][0] = ss - sy * cc;
		rot[2][1] = sc + sy * cs;
		rot[2][2] = cx * cy;
		rot[0][3] = rot[1][3] = rot[2][3] = 0.f;

		const auto farClip{ ram.read<float>(cameraPtr + 0x1E8) };
		writeProjectionMatrix(cameraPtr, m_fov, 20.f, farClip);
		ram.write(cameraPtr + 0x40, m_position);
		ram.write(cameraPtr + 0x80, rot);
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& offset{ m_game->offset() };
		const auto jalUpdateCmMat4{ Mips::jal(offset.Fn_UpdateCmMat4__FP2CM) };

		m_game->ram().writeConditional(enable,
			offset.Fn_SetCm__FP2CMP6VECTORT1ff + 0x50, 0x7E020040, 0x00000000, // Pos
			offset.Fn_SetCm__FP2CMP6VECTORT1ff + 0xC8, Mips::jal(offset.Fn_LoadRotateMatrixPanTilt__FffP7MATRIX3), 0x00000000, // Rot
			offset.Fn_RecalcCmFrustrum__FP2CM + 0x44, 0xE60001F4, 0x00000000,
			offset.Fn_RecalcCmFrustrum__FP2CM + 0x50, 0xE60C01F0, 0x00000000,
			offset.Fn_RecalcCmFrustrum__FP2CM + 0xB0, Mips::jal(offset.Fn_BuildSimpleProjectionMatrix__FffffffP7MATRIX4), 0x00000000,

			offset.Fn_SetCmLookAtSmooth__FP2CMiP6VECTORT2P2SOffffff + 0x3BC,
				std::array<Mips_t, 5>{ 0x7E060040, 0x7E020080, 0x7E030090, jalUpdateCmMat4, 0x7E0500A0 },
				std::array<Mips_t, 5>{ 0x00000000, 0x00000000, 0x00000000, jalUpdateCmMat4, 0x00000000 }, // Pos Rot
		
			offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0x74, 0x450103E5, 0x00000000, // Force recalc if game paused
			offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xEA8, 0xFAA10040, 0x00000000, // Pos
			offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF80, 0xFAA70080, 0x00000000, // Rot
			offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF88, 0x7EA20090, 0x00000000, // Rot
			offset.Fn_UpdateCplook__FP6CPLOOKP6CPDEFIP3JOYf + 0xF94, 0x7EA300A0, 0x00000000, // Rot
			offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x18, 0x7E020040, 0x00000000, // Pos
			offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x28, 0x7E020080, 0x00000000, // Rot
			offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x30, 0x7E030090, 0x00000000, // Rot
			offset.Fn_SetCmPosMat__FP2CMP6VECTORP7MATRIX3 + 0x38, 0x7E0200A0, 0x00000000, // Rot
			offset.Fn_UpdateCmMat4__FP2CM + 0x48, 0x4500000A, 0x00000000 // Boss frustrum
		);
	}
}
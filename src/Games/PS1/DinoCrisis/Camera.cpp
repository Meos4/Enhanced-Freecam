#include "Camera.hpp"

#include "Common/PS1/libgte.hpp"
#include "Common/PS1/PS1.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "Game.hpp"

#include <cmath>

namespace PS1::DinoCrisis
{
	static constexpr auto posMultiplyScalar{ 16.f };

	static constexpr auto
		fovMin{ 0.20f },
		fovMax{ 2.f };

	Camera::Camera(Game* game)
		: m_game(game)
	{
	}

	void Camera::draw()
	{
		CameraModel::drawPosition(&m_position, g_settings.dragFloatSpeed * posMultiplyScalar, !m_isEnabled);
		CameraModel::drawRotation(&m_euler.pitch, &m_euler.yaw, &m_euler.roll, !m_isEnabled);
		CameraModel::drawFov(&m_fov, !m_isEnabled, fovMin, fovMax);
	}

	void Camera::update()
	{
		enableGameCamera(!m_isEnabled);
		m_isEnabled ? write() : read();
	}

	void Camera::enable(bool enable)
	{
		m_isEnabled = enable;
	}

	void Camera::moveForward(float amount)
	{
		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) };

		amount *= posMultiplyScalar;

		m_positionFp.x += -cx * sy * amount;
		m_positionFp.y += sx * amount;
		m_positionFp.z += cx * cy * amount;
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
			ss{ sz * sx };

		amount *= posMultiplyScalar;

		m_positionFp.x += (cz * cy - ss * sy) * amount;
		m_positionFp.y += -cx * sz * amount;
		m_positionFp.z += (cz * sy + ss * cy) * amount;
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

		amount *= posMultiplyScalar;

		m_positionFp.x -= (sz * cy + cs * sy) * amount;
		m_positionFp.y -= cz * cx * amount;
		m_positionFp.z -= (sz * sy - cs * cy) * amount;
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

	void Camera::increaseFov(float amount)
	{
		CameraModel::increaseFov(&m_fov, amount, fovMin, fovMax);
	}

	void Camera::read()
	{
		const auto& ram{ m_game->ram() };
		const auto offsetMC{ m_game->offset().mainCamera };

		m_fov = 1.f;

		libgte::MATRIX view;
		ram.read(offsetMC, &view);
		ram.read(offsetMC + 0x30, &m_position);

		m_euler.roll = std::atan2(fixedToFloat(-view.m[0][1]), fixedToFloat(view.m[1][1]));
		m_euler.pitch = std::asin(fixedToFloat(view.m[2][1]));
		m_euler.yaw = std::atan2(fixedToFloat(-view.m[2][0]), fixedToFloat(view.m[2][2]));

		const auto
			sx{ std::sin(m_euler.pitch) },
			cx{ std::cos(m_euler.pitch) },
			sy{ std::sin(m_euler.yaw) },
			cy{ std::cos(m_euler.yaw) };

		m_position.x -= -cx * sy * view.t[2];
		m_position.y -= sx * view.t[2];
		m_position.z -= cx * cy * view.t[2];
	}

	void Camera::write()
	{
		const Vec3<s16> decFp{ s16(m_positionFp.x), s16(m_positionFp.y), s16(m_positionFp.z) };
		m_positionFp.x -= decFp.x;
		m_positionFp.y -= decFp.y;
		m_positionFp.z -= decFp.z;
		m_position += decFp;

		const auto
			sx{ static_cast<s16>(floatToFixed(std::sin(m_euler.pitch))) },
			cx{ static_cast<s16>(floatToFixed(std::cos(m_euler.pitch))) },
			sy{ static_cast<s16>(floatToFixed(std::sin(m_euler.yaw))) },
			cy{ static_cast<s16>(floatToFixed(std::cos(m_euler.yaw))) },
			sz{ static_cast<s16>(floatToFixed(std::sin(m_euler.roll))) },
			cz{ static_cast<s16>(floatToFixed(std::cos(m_euler.roll))) },
			cc{ static_cast<s16>((cz * cy) >> 12) },
			cs{ static_cast<s16>((cz * sy) >> 12) },
			sc{ static_cast<s16>((sz * cy) >> 12) },
			ss{ static_cast<s16>((sz * sy) >> 12) };

		libgte::MATRIX view{};
		view.m[0][0] = cc - ((ss * sx) >> 12);
		view.m[0][1] = (-cx * sz) >> 12;
		view.m[0][2] = cs + ((sc * sx) >> 12);
		view.m[1][0] = sc + ((cs * sx) >> 12);
		view.m[1][1] = (cz * cx) >> 12;
		view.m[1][2] = ss - ((cc * sx) >> 12);
		view.m[2][0] = (-cx * sy) >> 12;
		view.m[2][1] = sx;
		view.m[2][2] = (cx * cy) >> 12;

		const auto fov{ static_cast<s16>(floatToFixed(m_fov)) };

		view.m[0][0] = view.m[0][0] * fov >> 12;
		view.m[0][1] = view.m[0][1] * fov >> 12;
		view.m[0][2] = view.m[0][2] * fov >> 12;
		view.m[1][0] = view.m[1][0] * fov >> 12;
		view.m[1][1] = view.m[1][1] * fov >> 12;
		view.m[1][2] = view.m[1][2] * fov >> 12;

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		auto writeCamera = [&](u32 ptr)
		{
			ram.write(ptr, view);
			ram.write(ptr + 0x30, m_position);
			ram.write(ptr + 0x3C, s16(0)); // Shadow Forward
		};

		writeCamera(offset.mainCamera);
		writeCamera(offset.dinosaurCamera);
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& offset{ m_game->offset() };
		
		m_game->ram().writeConditional(enable,
			offset.Fn_updateMainCamera + 0x60, std::array<Mips_t, 3>{ 0xAC430014, 0xAC440018, 0xAC45001C }, std::array<Mips_t, 3>{ 0, 0, 0 }, // Eye
			offset.Fn_setCameraPosition, std::array<Mips_t, 2>{ 0x27BDFFA0, 0xAFB70054 }, Mips::jrRaNop(),
			offset.Fn_updateCamera + 0xF8, Mips::jal(offset.Fn_RotMatrix), 0x00000000,
			offset.Fn_setNextFrameCamera + 0x12C, 0xA622003C, 0x00000000, // Shadow Forward
			offset.Fn_moveNextCamera, std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 }, Mips::jrRaNop(), // Cutscene Position
			offset.Fn_updateLinearCutsceneCamera + 0x74, 0xA623003C, 0x00000000, // Shadow Forward
			offset.Fn_updateGameOver + 0x64, 0xA604003C, 0x00000000, // Shadow Forward
			offset.Fn_dinosaurEntranceCameraTransition + 0x138, 0xA6020030, 0x00000000, // X
			offset.Fn_dinosaurEntranceCameraTransition + 0x144, 0xA6020032, 0x00000000, // Y
			offset.Fn_dinosaurEntranceCameraTransition + 0x150, 0xA6020034, 0x00000000  // Z
		);
	}
}
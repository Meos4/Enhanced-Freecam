#include "Camera.hpp"

#include "Common/PS1/libgte.hpp"
#include "Common/PS1/PS1.hpp"

#include "Common/CameraModel.hpp"
#include "Common/Mips.hpp"
#include "Common/Settings.hpp"

#include "CustomCode.hpp"
#include "Game.hpp"

#include <cmath>

namespace PS1::ApeEscape
{
	static constexpr auto posMultiplyScalar{ 16.f };

	static constexpr auto
		fovMin{ 0.20f },
		fovMax{ 4.f };

	void Camera::writeProjectionMatrix(s16 x, s16 y)
	{
		libgte::MATRIX p{};
		p.m[0][0] = x;
		p.m[1][1] = y;
		p.m[2][2] = 4096;
		m_game->ram().write(m_game->offset().projectionMatrix, p.m);
	}

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
		if (!enable)
		{
			writeProjectionMatrix(4096, 3040);
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
		const auto state{ m_game->state() };

		m_fov = 1.f;

		if (state == State::SpaceMenu)
		{
			m_position = {};
			m_euler = {};
			return;
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		libgte::MATRIX view;
		ram.read(offset.viewMatrix, &view);

		for (s32 i{}; i < 3; ++i)
		{
			auto* const vec{ (libgte::SVECTOR*)&view.m[i][0] };
			libgte::VectorNormalSS(vec, vec);
		}

		m_euler.roll = std::atan2(fixedToFloat(-view.m[0][1]), fixedToFloat(view.m[1][1]));
		m_euler.pitch = std::asin(fixedToFloat(view.m[2][1]));
		m_euler.yaw = std::atan2(fixedToFloat(-view.m[2][0]), fixedToFloat(view.m[2][2]));

		auto cutscenePosition = [&]()
		{
			// Not 100% accurate but better than nothing
			auto* const v{ (s16*)&view.m };
			static constexpr auto yRatio{ floatToFixed(4096.f / 3040.f) };

			for (s32 i{}; i < 3; ++i)
			{
				v[3 + i] = (v[3 + i] * yRatio) >> 12;
			}

			for (s32 i{}; i < 9; ++i)
			{
				v[i] = -v[i];
			}

			m_position = {};

			for (s32 i{}; i < 3; ++i)
			{
				m_position.x += (view.m[i][0] * view.t[i]) >> 12;
				m_position.y += (view.m[i][1] * view.t[i]) >> 12;
				m_position.z += (view.m[i][2] * view.t[i]) >> 12;
			}
		};

		const auto version{ m_game->version() };

		u32 cShift,
			csShift,
			tpShift;

		if (version == Version::NtscU)
		{
			cShift = 0x59A8;
			csShift = 0x3B30;
			tpShift = 0x2CB0;
		}
		else if (version == Version::NtscJ)
		{
			cShift = 0x5A18;
			csShift = 0x3BF0;
			tpShift = 0x2CB0;
		}
		else
		{
			cShift = 0x5A68;
			csShift = 0x3C40;
			tpShift = 0x2CF0;
		}

		switch (state)
		{
		case State::Ingame:
		case State::TitleScreen:
			ram.read(offset.cameraPosition, &m_position); break;
		case State::TrainingPreview:
			ram.read(offset.overlay + tpShift, &m_position); break;
		case State::Cutscene:
		case State::StagePreview:
			ram.read(offset.overlay + cShift, &m_position); break;
		case State::ClearStage:
			ram.read(offset.overlay + csShift, &m_position); break;
		case State::AllVideo:
		case State::IngameCutscene:
			cutscenePosition(); break;
		case State::RaceResult:
			m_position = {}; break;
		case State::StageSelect:
			ram.read(0x001FFF90, &m_position); break;
		}
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
		view.t[0] = -m_position.x;
		view.t[1] = -m_position.y;
		view.t[2] = -m_position.z;

		auto* const eye{ (libgte::VECTOR*)&view.t };
		libgte::ApplyMatrixLV(&view, eye, eye);

		const auto 
			xFov{ static_cast<s16>(floatToFixed(m_fov)) },
			yFov{ static_cast<s16>(floatToFixed(m_fov * 0.7421875f)) };

		view.m[0][0] = view.m[0][0] * xFov >> 12;
		view.m[0][1] = view.m[0][1] * xFov >> 12;
		view.m[0][2] = view.m[0][2] * xFov >> 12;
		view.m[1][0] = view.m[1][0] * yFov >> 12;
		view.m[1][1] = view.m[1][1] * yFov >> 12;
		view.m[1][2] = view.m[1][2] * yFov >> 12;
		eye->vx = eye->vx * xFov >> 12;
		eye->vy = eye->vy * yFov >> 12;

		libgte::MATRIX billb{};
		billb.m[0][0] = cy;
		billb.m[0][2] = -sy;
		billb.m[1][1] = fixedOne;
		billb.m[2][0] = sy;
		billb.m[2][2] = cy;

		libgte::MulMatrix0(&view, &billb, &billb);

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		if (m_game->state() == State::Ingame)
		{
			ram.write(CustomCode::viewMatrixOffset(*m_game), view);
		}
		else
		{
			ram.write(offset.viewMatrix, view);
		}

		ram.write(offset.billboardMatrix, billb.m);
		writeProjectionMatrix(xFov, yFov);
	}

	void Camera::enableGameCamera(bool enable)
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto state{ m_game->state() };
		const auto version{ m_game->version() };

		u32 avShift,
			ssShift,
			tpShift;

		if (version == Version::NtscU)
		{
			avShift = 0x200C;
			ssShift = 0x2560;
			tpShift = 0x20D8;
		}
		else if (version == Version::NtscJ)
		{
			avShift = 0x209C;
			ssShift = 0x2524;
			tpShift = 0x214C;
		}
		else
		{
			avShift = 0x20BC;
			ssShift = 0x2524;
			tpShift = 0x214C;
		}

		if (state != State::Ingame || enable)
		{
			ram.write(offset.Fn_setViewMatrix, std::array<Mips_t, 2>{ 0x27BDFF90, 0xAFB50064 });
		}
		else if (state == State::Ingame && CustomCode::isApplied(*m_game) && !enable)
		{
			ram.write(offset.Fn_setViewMatrix, std::array<Mips_t, 2>{ Mips::j(offset.sonyLibrary), 0x00C02021 });
		}

		if (state == State::IngameCutscene || state == State::AllVideo)
		{
			ram.write(offset.overlay + avShift, enable ? std::array<Mips_t, 2>{ 0x27BDFFE8, 0xAFB00010 } : Mips::jrRaNop());
		}
		else if (state == State::TitleScreen)
		{
			ram.write(offset.Fn_setViewMatrix, enable ? std::array<Mips_t, 2>{ 0x27BDFF90, 0xAFB50064 } : Mips::jrRaNop());
		}
		else if (state == State::Cutscene || state == State::StagePreview || state == State::ClearStage || state == State::RaceResult)
		{
			ram.write(offset.overlay, enable ? std::array<Mips_t, 2>{ 0x27BDFF88, 0xAFB10064 } : Mips::jrRaNop());
		}
		else if (state == State::StageSelect)
		{
			ram.writeConditional(enable,
				offset.overlay + ssShift, std::array<Mips_t, 2>{ 0x27BDFFB8, 0xAFB10034 }, Mips::jrRaNop(),
				// Prevent breakpoints from div by zero when close to the "CLEARED" label
				offset.overlay + 0x970, 0x0007000D, 0x00000000,
				offset.overlay + 0x9A4, 0x0007000D, 0x00000000
			);
		}
		else if (state == State::SpaceMenu)
		{
			ram.write(offset.overlay + 0x8E8, enable ? std::array<Mips_t, 2>{ 0x27BDFFB8, 0xAFB10034 } : Mips::jrRaNop());
		}
		else if (state == State::TrainingPreview)
		{
			ram.writeConditional(enable,
				offset.overlay, std::array<Mips_t, 2>{ 0x27BDFF88, 0xAFB10064 }, Mips::jrRaNop(), 
				offset.overlay + tpShift, std::array<Mips_t, 2>{ 0x27BDFF88, 0x240408C8}, Mips::jrRaNop()
			);
		}
	}
}
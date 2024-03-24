#include "Freecam.hpp"

#include "Common/FreecamModel.hpp"
#include "Common/Renderer.hpp"
#include "Common/Settings.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::HauntingGround
{
	static constexpr Input3D input3D
	{
		.forward = Input::MoveForward,
		.backward = Input::MoveBackward,
		.right = Input::MoveRight,
		.left = Input::MoveLeft,
		.up = Input::MoveUp,
		.down = Input::MoveDown,
		.rotateRollPos = Input::RotateZPos,
		.rotateRollNeg = Input::RotateZNeg,
		.rotatePitchPos = Input::RotateXPos,
		.rotatePitchNeg = Input::RotateXNeg,
		.rotateYawPos = Input::RotateYPos,
		.rotateYawNeg = Input::RotateYNeg,
		.fovPos = Input::FovPos,
		.fovNeg = Input::FovNeg
	};

	Freecam::Freecam(Game* game)
		: m_game(game), m_camera(game), m_misc(game), m_controller(game), m_controls(m_game->input(), input3D)
	{
	}

	void Freecam::draw()
	{
		Ui::setXSpacingStr("Movement Speed");
		FreecamModel::drawCameraName(FreecamModel::cameraNameMonoState);

		if (FreecamModel::drawEnable(&m_isEnabled))
		{
			enable(m_isEnabled);
		}

		FreecamModel::drawSeparatorProperties();
		m_camera.draw();
		FreecamModel::drawSeparatorOthers();
		m_misc.draw();
		m_controller.draw();
		FreecamModel::draw3DSettings();
	}

	void Freecam::update()
	{
		auto* const input{ m_game->input() };

		m_controller.update();
		m_camera.update();
		m_misc.update();

		if (input->isPressed(Input::ToggleFreecam))
		{
			enable(!m_isEnabled);
		}

		if (m_isEnabled)
		{
			FreecamModel::update3DSettings(input, 
				Input::MovementSpeedPos, Input::MovementSpeedNeg,
				Input::RotationSpeedPos, Input::RotationSpeedNeg);

			const auto dt{ Renderer::deltaTime() };

			m_camera.moveForward(m_controls.forwardVelocity(dt));
			m_camera.moveRight(m_controls.rightVelocity(dt));
			m_camera.moveUp(m_controls.upVelocity(dt));
			m_camera.rotateX(m_controls.pitchVelocity(dt));
			m_camera.rotateY(m_controls.yawVelocity(dt));
			m_camera.rotateZ(m_controls.rollVelocity(dt));
			m_camera.increaseFov(m_controls.fovVelocity(dt));
		}
	}

	void Freecam::enable(bool enable)
	{
		m_isEnabled = enable;
		m_camera.enable(enable);
		m_misc.enable(enable);
		m_controller.enable(enable);

		if (enable)
		{
			auto* const settings{ m_game->settings() };

			if (settings->resetMovementSpeed)
			{
				g_settings.movementSpeed = g_settings.movementSpeedDefault;
			}
			if (settings->resetRotationSpeed)
			{
				g_settings.rotationSpeed = g_settings.rotationSpeedDefault;
			}

			m_controls.resetVelocity();
		}
	}
}
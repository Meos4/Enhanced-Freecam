#include "Freecam.hpp"

#include "Common/FreecamModel.hpp"
#include "Common/Renderer.hpp"
#include "Common/Settings.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::DBZBudokai3
{
	Freecam::Freecam(Game* game)
		: m_game(game), m_camera(game), m_misc(game), m_controller(game), m_controls(m_game->input())
	{
	}

	void Freecam::draw()
	{
		Ui::setXSpacingStr("Movement Speed");
		FreecamModel::drawCameraName(Game::stateNames(), m_game->state());

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
				Input::RotationSpeedPos, Input::RotationSpeedNeg,
				Input::FovSpeedPos, Input::FovSpeedNeg);

			m_camera.moveForward(m_controls.forwardVelocity(Input::MoveForward, Input::MoveBackward));
			m_camera.moveRight(m_controls.rightVelocity(Input::MoveRight, Input::MoveLeft));
			m_camera.moveUp(m_controls.upVelocity(Input::MoveUp, Input::MoveDown));
			m_camera.rotateX(m_controls.pitchVelocity(Input::RotateXPos, Input::RotateXNeg));
			m_camera.rotateY(m_controls.yawVelocity(Input::RotateYPos, Input::RotateYNeg));
			m_camera.rotateZ(m_controls.rollVelocity(Input::RotateZPos, Input::RotateZNeg));
			m_camera.increaseFov(m_controls.fovVelocity(Input::FovPos, Input::FovNeg));
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
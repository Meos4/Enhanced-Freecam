#include "Controller.hpp"

#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::Sly1
{
	Controller::Controller(Game* game)
		: m_game(game)
	{
	}

	void Controller::draw()
	{
		const std::array<Ui::LabelFlag, 2> lf
		{
			"Button", &m_isButtonEnabled,
			"Joystick", &m_isJoystickEnabled
		};

		MiscModel::drawFlags("Controller", lf, !m_isEnabled);
	}

	void Controller::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.write(offset.Fn_FReadJoy__FP3JOY + 0x60, m_isButtonEnabled ? 0xA62200A8 : 0xA62000A8);

		ram.writeConditional(m_isJoystickEnabled,
			offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1B4, 0xE5220000, 0xAD200000,
			offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1E4, 0xE4E00000, 0xACE00000,
			offset.Fn_GetJoyXYDeflection__FP3JOYUcUcPfN23PUcT6PiP2LM + 0x1EC, 0xE5010000, 0xAD000000
		);
	}

	void Controller::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			auto* const settings{ m_game->settings() };

			if (settings->disableButton)
			{
				m_isButtonEnabled = false;
			}
			if (settings->disableJoystick)
			{
				m_isJoystickEnabled = false;
			}
		}
		else
		{
			m_isButtonEnabled = true;
			m_isJoystickEnabled = true;
		}
	}
}
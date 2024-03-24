#include "Controller.hpp"

#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS1::ApeEscape
{
	Controller::Controller(Game* game)
		: m_game(game)
	{
	}

	void Controller::draw()
	{
		const std::array<Ui::LabelFlag, 3> lf
		{
			"Button", &m_isButtonEnabled,
			"L Joystick", &m_isLJoystickEnabled,
			"R Joystick", &m_isRJoystickEnabled
		};

		MiscModel::drawFlags("Controller", lf, !m_isEnabled);
	}

	void Controller::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.writeConditional(m_isButtonEnabled,
			offset.Fn_padStatus + 0x1B8, 0xAE240158, 0xAE200158,
			offset.Fn_padStatus + 0x1C0, 0xAE220160, 0xAE200160
		);

		ram.write(offset.Fn_padStatus + 0x464, m_isLJoystickEnabled ? 0xA6420180 : 0xA6400180);
		
		ram.writeConditional(m_isRJoystickEnabled,
			offset.Fn_padStatus + 0x3DC, 0xA6420182, 0xA6400182,
			offset.Fn_padStatus + 0x3F0, 0xA6420182, 0xA6400182
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
			if (settings->disableLJoystick)
			{
				m_isLJoystickEnabled = false;
			}
			if (settings->disableRJoystick)
			{
				m_isRJoystickEnabled = false;
			}
		}
		else
		{
			m_isButtonEnabled = true;
			m_isLJoystickEnabled = true;
			m_isRJoystickEnabled = true;
		}
	}
}
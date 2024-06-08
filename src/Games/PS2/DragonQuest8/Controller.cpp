#include "Controller.hpp"

#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DragonQuest8
{
	Controller::Controller(Game* game)
		: m_game(game)
	{
	}

	void Controller::draw()
	{
		MiscModel::drawControllerButtonJoystick(&m_isButtonEnabled, &m_isJoystickEnabled, !m_isEnabled);
	}

	void Controller::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		if (m_isEnabled)
		{
			auto* const input{ m_game->input() };
			MiscModel::toggle(input, Input::Button, &m_isButtonEnabled);
			MiscModel::toggle(input, Input::Joystick, &m_isJoystickEnabled);
		}

		if (m_game->version() == Version::Pal)
		{
			ram.write(offset.Fn_padStatus + 0xDC, m_isButtonEnabled ? 0x3263FFFF : 0x00001821);
			ram.writeConditional(m_isJoystickEnabled,
				offset.Fn_padStatus + 0x108, 0x93A20092, 0x2402007F,
				offset.Fn_padStatus + 0x110, 0x93A20093, 0x2402007F,
				offset.Fn_padStatus + 0x118, 0x93A20094, 0x2402007F,
				offset.Fn_padStatus + 0x120, 0x93A20095, 0x2402007F
			);
		}
		else
		{
			ram.write(offset.Fn_padStatus + 0x70, m_isButtonEnabled ? 0x3043FFFF : 0x00001821);
			ram.writeConditional(m_isJoystickEnabled,
				offset.Fn_padStatus + 0x80, 0x93A20034, 0x2402007F,
				offset.Fn_padStatus + 0x88, 0x93A20035, 0x2402007F,
				offset.Fn_padStatus + 0x90, 0x93A20036, 0x2402007F,
				offset.Fn_padStatus + 0x98, 0x93A20037, 0x2402007F
			);
		}
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
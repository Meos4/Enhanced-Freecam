#include "Controller.hpp"

#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DBZBudokai3
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

		ram.writeConditional(m_isButtonEnabled,
			offset.Fn_padStatus + 0x60, 0xA623000E, 0xA620000E,
			offset.Fn_padStatus + 0x70, 0xA624000C, 0xA620000C
		);

		ram.writeConditional(m_isJoystickEnabled,
			offset.Fn_padStatus + 0x98, 0xA2230018, 0xA2200018,
			offset.Fn_padStatus + 0xA4, 0xA2230019, 0xA2200019,
			offset.Fn_padStatus + 0xB0, 0xA223001A, 0xA220001A,
			offset.Fn_padStatus + 0xBC, 0xA223001B, 0xA220001B
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
#include "Controller.hpp"

#include "Common/MiscModel.hpp" 

#include "Game.hpp"

namespace PS2::JadeCocoon2
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

		ram.writeConditional(m_isButtonEnabled,
			offset.Fn__gnkPadman_StandardStatus + 0x30, 0x8FA40040, 0x00002021,
			offset.Fn__gnkPadman_StandardStatus + 0x44, 0x8C840000, 0x00002021
		);

		ram.write(offset.Fn__gnkPadman_StandardStatus + 0x188, m_isJoystickEnabled ? 0x90640000 : 0x2404007F);
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
#include "Controller.hpp"

#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DBZTenkaichi3
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

		ram.write(offset.Fn_padStatus + 0x2F0, m_isButtonEnabled ? 0xAE300148 : 0xAE200148);

		ram.writeConditional(m_isJoystickEnabled,
			offset.Fn_padStatus + 0x108, 0xE6200138, 0xAE200138,
			offset.Fn_padStatus + 0x118, 0xE620013C, 0xAE20013C,
			offset.Fn_padStatus + 0x128, 0xE6200130, 0xAE200130,
			offset.Fn_padStatus + 0x138, 0xE6200134, 0xAE200134
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
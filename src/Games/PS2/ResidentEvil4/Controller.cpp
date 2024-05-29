#include "Controller.hpp"

#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::ResidentEvil4
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
		const auto offsetPS{ m_game->offset().Fn_padStatus };

		ram.writeConditional(m_isButtonEnabled,
			offsetPS + 0x178, 0xAE0C0010, 0xAE000010,
			offsetPS + 0x188, 0xAE020010, 0xAE000010,
			offsetPS + 0x1A0, 0xAE020010, 0xAE000010
		);

		ram.writeConditional(m_isJoystickEnabled,
			offsetPS + 0x158, 0xA2070000, 0xA2000000,
			offsetPS + 0x160, 0xA2020001, 0xA2000001,
			offsetPS + 0x164, 0xA2030002, 0xA2000002,
			offsetPS + 0x168, 0xA2040003, 0xA2000003
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
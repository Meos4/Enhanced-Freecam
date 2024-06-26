#include "Controller.hpp"

#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS1::DinoCrisis
{
	Controller::Controller(Game* game)
		: m_game(game)
	{
	}

	void Controller::draw()
	{
		MiscModel::drawControllerButton(&m_isButtonEnabled, !m_isEnabled);
	}

	void Controller::update()
	{
		if (m_isEnabled)
		{
			MiscModel::toggle(m_game->input(), Input::Button, &m_isButtonEnabled);
		}

		const auto offsetPS{ m_game->offset().Fn_padStatus };
		const auto psShift{ m_game->version() == Version::NtscJ ? 0x114 : 0x108 };

		m_game->ram().writeConditional(m_isButtonEnabled,
			offsetPS + psShift, 0xA4A70000, 0xA4A00000,
			offsetPS + psShift + 0x10, 0xA4A20002, 0xA4A00002
		);
	}

	void Controller::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			if (m_game->settings()->disableButton)
			{
				m_isButtonEnabled = false;
			}
		}
		else
		{
			m_isButtonEnabled = true;
		}
	}
}
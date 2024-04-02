#include "Controller.hpp"

#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS1::DinoCrisis
{
	Controller::Controller(Game* game)
		: m_game(game)
	{
	}

	void Controller::draw()
	{
		const Ui::LabelFlag lf{ "Button", &m_isButtonEnabled };
		MiscModel::drawFlags("Controller", { &lf, 1 }, !m_isEnabled);
	}

	void Controller::update()
	{
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
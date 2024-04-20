#include "Controller.hpp"

#include "Common/Mips.hpp"
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
		const auto state{ m_game->state() };

		ram.writeConditional(m_isButtonEnabled,
			offset.Fn_padStatus + 0x1B8, 0xAE240158, 0xAE200158,
			offset.Fn_padStatus + 0x1C0, 0xAE220160, 0xAE200160
		);

		ram.write(offset.Fn_padStatus + 0x464, m_isLJoystickEnabled ? 0xA6420180 : 0xA6400180);
		
		ram.writeConditional(m_isRJoystickEnabled,
			offset.Fn_padStatus + 0x3DC, 0xA6420182, 0xA6400182,
			offset.Fn_padStatus + 0x3F0, 0xA6420182, 0xA6400182
		);

		if (state == State::SpecterBoxing)
		{
			const auto version{ m_game->version() };

			u32 bShift;
			u32 ljXStructShift;

			if (version == Version::NtscU)
			{
				bShift = 0x135B8;
				ljXStructShift = 0x13C;		
			}
			else
			{
				bShift = 0x134C8;
				ljXStructShift = 0x154;
			}

			const auto
				bStructShift{ ljXStructShift + 0x38 },
				ljYStructShift{ ljXStructShift + 4 },
				rjXStructShift{ ljYStructShift + 4 },
				rjYStructShift{ rjXStructShift + 4 };

			ram.write(offset.minigame + bShift, m_isButtonEnabled ? 0xAF820000 + bStructShift : 0xAF800000 + bStructShift);
			ram.write(offset.minigame + bShift + 0x1104, m_isLJoystickEnabled ? 
				std::array<Mips_t, 2>{ 0xA7830000 + ljXStructShift, 0xA7820000 + ljYStructShift } : 
				std::array<Mips_t, 2>{ 0xA7800000 + ljXStructShift, 0xA7800000 + ljYStructShift }
			);

			ram.write(offset.minigame + bShift + 0x1128, m_isRJoystickEnabled ? 
				std::array<Mips_t, 2>{ 0xA7830000 + rjXStructShift, 0xA7820000 + rjYStructShift } : 
				std::array<Mips_t, 2>{ 0xA7800000 + rjXStructShift, 0xA7800000 + rjYStructShift }
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
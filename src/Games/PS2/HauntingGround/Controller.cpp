#include "Controller.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::HauntingGround
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
			offset.Fn_padStatus + 0xE4, 0xAE270004, 0xAE200004, // Up
			offset.Fn_padStatus + 0x11C, 0xAE260004, 0xAE200004, // Right
			offset.Fn_padStatus + 0x14C, 0xAE230004, 0xAE200004, // Left
			offset.Fn_padStatus + 0x188, 0xAE220004, 0xAE200004, // Down
			offset.Fn_padStatus + 0x1C4, 0xAE240004, 0xAE200004, // L1
			offset.Fn_padStatus + 0x1EC, 0xAE230004, 0xAE200004, // R1
			offset.Fn_padStatus + 0x36C, 0xA6650014, 0xA6600014, // Button
			offset.Fn_padStatus + 0x394, 0xA665001C, 0xA660001C, // Button Temp
			offset.Fn_padCrossTriangle + 0x58, 0xAC680004, 0xAC600004, // Cross
			offset.Fn_padCrossTriangle + 0x7C, 0xAC670004, 0xAC600004 // Triangle
		);

		const auto jalSceVu0ScaleVectorXYZ{ Mips::jal(offset.Fn_sceVu0ScaleVectorXYZ) };
		ram.write(offset.Fn_padStatus + 0x5A0, m_isLJoystickEnabled ? jalSceVu0ScaleVectorXYZ : 0x7C800000);
		ram.write(offset.Fn_padStatus + 0x718, m_isRJoystickEnabled ? jalSceVu0ScaleVectorXYZ : 0x7C800000);
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
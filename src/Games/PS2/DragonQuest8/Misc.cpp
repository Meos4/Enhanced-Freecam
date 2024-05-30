#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DragonQuest8
{
	Misc::Misc(Game* game)
		: m_game(game)
	{
	}

	void Misc::draw()
	{
		MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
	}

	void Misc::update()
	{
		if (m_isEnabled)
		{
			auto* const input{ m_game->input() };

			if (input->isPressed(Input::PauseGame))
			{
				m_isGamePaused = !m_isGamePaused;
			}
			if (input->isPressed(Input::HideHud))
			{
				m_isHudHidden = !m_isHudHidden;
			}
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		u32 ubShift;

		Mips_t
			tInstr,
			dchInstr,
			dchInstr2,
			dbhInstr,
			dbhInstr2;

		if (m_game->version() == Version::Pal)
		{
			tInstr = 0xC60C385C;
			ubShift = 0xF0;
			dchInstr = 0x10000073;
			dchInstr2 = 0x12A00073;
			dbhInstr = 0x100000EF;
			dbhInstr2 = 0x104000EF;
		}
		else
		{
			tInstr = 0x3C023F80;
			ubShift = 0xE0;
			dchInstr = 0x10000069;
			dchInstr2 = 0x12600069;
			dbhInstr = 0x100000EB;
			dbhInstr2 = 0x104000EB;
		}

		ram.writeConditional(m_isGamePaused,
			offset.Fn_npcStep + 0xB4, 0x44806000, tInstr,
			offset.Fn_playerStep + 0x88, 0x44806000, tInstr,
			offset.walkSpeed, 0.f, 3.8f,
			offset.Fn_updateBattle + ubShift, 0x00000000, 0x50400003, // Next step anim
			offset.Fn_npcScript + 0x10, 0x1000000A, 0x1060000A
		);

		ram.writeConditional(m_isHudHidden,
			offset.Fn_drawCommonHud + 0x28, dchInstr, dchInstr2,
			offset.Fn_drawBattleHud + 0x3C, dbhInstr, dbhInstr2,
			offset.Fn_drawMidTextOnScreen + 0x98, 0x00000000, Mips::jal(offset.Fn_drawTextOnScreen)
		);
	}

	void Misc::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
		{
			auto* const settings{ m_game->settings() };

			if (settings->hideHud)
			{
				m_isHudHidden = true;
			}
			if (settings->pauseGame)
			{
				m_isGamePaused = true;
			}
		}
		else
		{
			m_isHudHidden = false;
			m_isGamePaused = false;
		}
	}
}
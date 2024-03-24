#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::Sly1
{
	Misc::Misc(Game* game)
		: m_game(game)
	{
	}

	void Misc::draw()
	{
		const std::array<Ui::LabelFlag, 2> lf
		{
			"Pause Game", &m_isGamePaused,
			"Hide Hud", &m_isHudHidden
		};

		MiscModel::drawFlags("Misc", lf, !m_isEnabled);
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

		ram.write(offset.Fn_MarkClockTick__FP5CLOCK + 0x98, m_isGamePaused ? 0x00001021 : 0x8E020000);
		
		std::array<Mips_t, 2>
			dlbInstr,
			dlInstr,
			dcInstr,
			dpInstr,
			dbInstr,
			dtInstr,
			dnInstr;

		if (m_game->version() == Version::NtscU)
		{
			dlbInstr = { 0x27BDFFB0, 0x3C014284 };
			dlInstr = { 0x27BDFF80, 0x7FB30040 };
			dcInstr = { 0x27BDFED0, 0x7FB10100 };
			dpInstr = { 0x27BDFDD0, 0x7FB20150 };
			dbInstr = { 0x27BDFB50, 0x7FB20400 };
			dtInstr = { 0x27BDFEB0, 0x7FB10100 };
			dnInstr = { 0x27BDFEC0, 0x7FB10100 };
		}
		else if (m_game->version() == Version::Pal)
		{
			dlbInstr = { 0x27BDFFB0, 0x3C014259 };
			dlInstr = { 0x27BDFF80, 0x7FB30040 };
			dcInstr = { 0x27BDFEB0, 0x7FB20120 };
			dpInstr = { 0x27BDFDC0, 0x7FB30170 };
			dbInstr = { 0x27BDF820, 0x7FB20730 };
			dtInstr = { 0x27BDFEA0, 0x7FB10110 };
			dnInstr = { 0x27BDFEA0, 0x7FB20120 };
		}
		else
		{
			dlbInstr = { 0x27BDFFB0, 0x3C014284 };
			dlInstr = { 0x27BDFE60, 0x7FB30140 };
			dcInstr = { 0x27BDFEB0, 0x7FB20120 };
			dpInstr = { 0x27BDFDC0, 0x7FB30170 };
			dbInstr = { 0x27BDF820, 0x7FB20730 };
			dtInstr = { 0x27BDFEA0, 0x7FB10110 };
			dnInstr = { 0x27BDFEA0, 0x7FB20120 };
		}

		ram.writeConditional(m_isHudHidden,
			offset.Fn_DrawLetterbox__FP9LETTERBOX, Mips::jrRaNop(), dlbInstr,
			offset.Fn_DrawLogo__FP4LOGO, Mips::jrRaNop(), dlInstr,
			offset.Fn_DrawAttract__FP7ATTRACT, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C020026, 0x3C014040 },
			offset.Fn_DrawCtr__FP3CTR, Mips::jrRaNop(), dcInstr,
			offset.Fn_DrawPrompt__FP6PROMPT, Mips::jrRaNop(), dpInstr,
			offset.Fn_DrawBinoc__FP5BINOC, Mips::jrRaNop(), dbInstr,
			offset.Fn_DrawTv__FP2TV, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C020026 },
			offset.Fn_DrawTitle__FP5TITLE, Mips::jrRaNop(), dtInstr,
			offset.Fn_DrawWmc__FP3WMC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFE80, 0x7FB300F0 },
			offset.Fn_DrawBossctr__FP7BOSSCTR, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF00, 0x7FB500B0 },
			offset.Fn_DrawNote__FP4NOTE, Mips::jrRaNop(), dnInstr
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
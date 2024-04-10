#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::DBZTenkaichi3
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
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto state{ m_game->state() };

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

		const auto flags{ ram.read<s32>(offset.flags) };
		const bool isPauseMenuEnabled{ (state == State::Battle || state == State::BattleCutscene) && flags & 0x4000 ? true : false };

		if (state != State::None)
		{
			ram.write(offset.flags, m_isGamePaused || isPauseMenuEnabled ? flags | 0x100 : flags & ~0x100);
		}

		ram.write(offset.Fn_battleDrawHud, state != State::None && m_isHudHidden ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 });
	}

	void Misc::enable(bool enable)
	{
		m_isEnabled = enable;

		if (m_isEnabled)
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
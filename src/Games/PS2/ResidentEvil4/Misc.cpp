#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvil4
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
			MiscModel::toggle(input, Input::PauseGame, &m_isGamePaused);
			MiscModel::toggle(input, Input::HideHud, &m_isHudHidden);
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.writeConditional(m_isGamePaused,
			offset.Fn_updatePlayer, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0x7FB00040 },
			offset.Fn_updateNpc, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			offset.Fn_updateExtern, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FB00010 },
			offset.Fn_updateAction, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C040032 },
			offset.Fn_updateSfx, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0x3C020032 },
			offset.Fn_updateSfx2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0x3C060032 },
			offset.Fn_updateItems, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			offset.Fn_updateGameOver + 0x634, 0x00000000, 0x26D60001,
			offset.Fn_updateTimer, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FB00010 }
		);

		ram.writeConditional(m_isHudHidden,
			offset.Fn_drawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x7FB00020 },
			offset.Fn_drawText, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0x7FB00030 }
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
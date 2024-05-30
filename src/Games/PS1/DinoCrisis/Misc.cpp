#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS1::DinoCrisis
{
	Misc::Misc(Game* game)
		: m_game(game)
	{
	}

	void Misc::draw()
	{
		MiscModel::drawMiscPauseGame(&m_isGamePaused, !m_isEnabled);
	}

	void Misc::update()
	{
		if (m_isEnabled)
		{
			if (m_game->input()->isPressed(Input::PauseGame))
			{
				m_isGamePaused = !m_isGamePaused;
			}
		}

		const auto& offset{ m_game->offset() };
		
		m_game->ram().writeConditional(m_isGamePaused,
			offset.Fn_updateMap, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 },
			offset.Fn_updateInteractable, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x3C021F80 },
			offset.Fn_updateCharacters, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 },
			offset.Fn_updateItems, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C031F80 },
			offset.Fn_updateSfx, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C021F80, 0x8C420000 }
		);
	}

	void Misc::enable(bool enable)
	{
		m_isEnabled = enable;

		if (m_isEnabled)
		{
			if (m_game->settings()->pauseGame)
			{
				m_isGamePaused = true;
			}
		}
		else
		{
			m_isGamePaused = false;
		}
	}
}
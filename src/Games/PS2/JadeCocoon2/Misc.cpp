#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::JadeCocoon2
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
		const auto version{ m_game->version() };

		std::array<Mips_t, 2> sdseInstr;
		Mips_t sdiInstr;

		if (version == Version::Pal)
		{
			sdseInstr = { 0x27BDFF10, 0x7FBF0010 };
			sdiInstr = 0x27BDFF60; 
		}
		else if (version == Version::NtscU)
		{
			sdseInstr = { 0x27BDFF10, 0x7FBF0010 };
			sdiInstr = 0x27BDFF70;
		}
		else
		{
			sdseInstr = { 0x27BDFEE0, 0x7FBF0040 };
			sdiInstr = 0x27BDFF70;
		}

		ram.write<s32>(offset.newPause, m_isGamePaused ? 1 : 0);
		ram.writeConditional(m_isHudHidden,
			offset.Fn_m2Sprite_Disp, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0x7FBF0000 },
			offset.Fn_m2Fix_SjisDispString, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			offset.Fn_m2Fix_SjisDispValue, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			offset.Fn_m2Fix_SjisDispStringEx, Mips::jrRaNop(), sdseInstr,
			offset.Fn_m2Window_DispWindow, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x7FBF0000 },
			offset.Fn_M2AutoMap_Draw, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFD10, 0x7FBF0090 },
			offset.Fn_m2Selector_DispIcon, Mips::jrRaNop(), std::array<Mips_t, 2>{ sdiInstr, 0x7FBF0030 }
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
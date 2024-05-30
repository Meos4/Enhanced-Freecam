#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DBZTenkaichi3
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
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto state{ m_game->state() };
		const auto version{ m_game->version() };

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

		ram.writeConditional(state != State::None && m_isGamePaused,
			offset.Fn_updateChar + 0x24, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600056, 0xDFB00020 }, // Frustrum
			offset.Fn_updateChar + 0xF0, 0x10000011, 0x14400011, // Anim
			offset.Fn_updateCharBlinkEyes, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB10008 },
			offset.Fn_updateCharVisibility + 0x30, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600029, 0xDFB00000 },
			offset.Fn_updateCharVisibility2 + 0x30, std::array<Mips_t, 2>{ 0x00000000, 0x00000000 }, std::array<Mips_t, 2>{ 0x54600035, 0xDFB00000 }
		);

		if (version != Version::NtscJ)
		{
			const auto usIntr{ version == Version::Pal ? 0x1460001D : 0x1460001F };
			ram.write(offset.Fn_updateShenron + 0x68, state != State::None && m_isGamePaused ? 0x00000000 : usIntr);
		}

		ram.writeConditional(state != State::None && m_isHudHidden, 
			offset.Fn_battleDrawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_drawSprite, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFB00000 }
		);
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
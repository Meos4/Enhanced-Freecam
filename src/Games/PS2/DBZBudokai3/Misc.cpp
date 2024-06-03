#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

#include "Game.hpp"

namespace PS2::DBZBudokai3
{
	static constexpr auto timescaleMin{ 0.1f }, timescaleMax{ 3.f };

	static float timescaleBase(s32 version)
	{
		return version == Version::Pal || version == Version::PalCollector ? 1.2f : 1.f;
	}

	Misc::Misc(Game* game)
		: m_game(game), m_timescale(timescaleBase(game->version()))
	{
	}

	void Misc::draw()
	{
		if (m_game->state() == State::DragonUniverse)
		{
			MiscModel::drawMiscHideHud(&m_isHudHidden, !m_isEnabled);
		}
		else
		{
			const auto base{ timescaleBase(m_game->version()) };
			MiscModel::drawTimescale(&m_timescale, timescaleMin * base, timescaleMax * base, !m_isEnabled);
			MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
		}
	}

	void Misc::update()
	{
		const auto version{ m_game->version() };
		const auto state{ m_game->state() };

		if (m_isEnabled)
		{
			auto* const input{ m_game->input() };
			const auto base{ timescaleBase(version) };

			if (input->isPressed(Input::PauseGame))
			{
				m_isGamePaused = !m_isGamePaused;
			}
			if (input->isPressed(Input::HideHud))
			{
				m_isHudHidden = !m_isHudHidden;
			}

			MiscModel::updateTimescale(input, Input::TimescalePos, Input::TimescaleNeg, &m_timescale, timescaleMin * base, timescaleMax * base);
			
			if (state == State::DragonUniverse)
			{
				m_isGamePaused = false;
				m_timescale = base;
			}
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		u32 bupmShift,
			bsdhShift,
			bddiShift;

		Mips_t
			bupmInstr,
			bddiInstr,
			bddiInstr2;

		if (version == Version::Pal || version == Version::NtscU)
		{
			bupmShift = 0x664;
			bsdhShift = 0x3C;
			bddiShift = 0x54;
			bupmInstr = 0xAC2088EC;
			bddiInstr = 0x10000010;
			bddiInstr2 = 0x10600010;
		}
		else if (version == Version::PalCollector || version == Version::NtscJ)
		{
			bupmShift = 0x674;
			bsdhShift = 0x3C;
			bddiShift = 0x54;
			bupmInstr = 0xAC20892C;
			bddiInstr = 0x10000010;
			bddiInstr2 = 0x10600010;
		}
		else
		{
			bupmShift = 0x6DC;
			bsdhShift = 0x38;
			bddiShift = 0x44;
			bupmInstr = 0xAC60892C;
			bddiInstr = 0x10000015;
			bddiInstr2 = 0x10600015;
		}

		const bool isPauseMenuEnabled{ ram.read<s16>(offset.battleIsPauseMenuEnabled) == 1 };

		ram.write(offset.Fn_battleUpdatePauseMenu + bupmShift, state == State::Battle && m_isGamePaused && !isPauseMenuEnabled ? 0x00000000 : bupmInstr);
		ram.writeConditional(state == State::Battle && m_isHudHidden,
			offset.Fn_battleShouldDrawHud + bsdhShift, 0x00001021, 0x00000000,
			offset.Fn_battleDrawDuelingInfo + bddiShift, bddiInstr, bddiInstr2
		);

		ram.writeConditional(state == State::DragonUniverse && m_isHudHidden,
			offset.Fn_duACTDRAW + 4, std::array<Mips_t, 2>{ 0x03E00008, 0x27BD0060 }, std::array<Mips_t, 2>{ 0xFFBF0020, 0x7FB10010 },
			offset.Fn_duMAP, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0x24030002 },
			offset.Fn_duBALL_DRAW, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x3C020003 },
			offset.Fn_duPOS, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF80, 0x24030002 }
		);

		if (state == State::Battle)
		{
			ram.write(offset.battleTimescale, m_timescale);

			if (m_isGamePaused && !isPauseMenuEnabled)
			{
				ram.write<s16>(offset.battleIsPauseMenuEnabled + 0x14, 0); // Type of menu
				ram.write<s32>(offset.battleIsPaused, 1);
			}
			else if (ram.read<s32>(offset.battleIsPaused + 0xC) == 5) // Tenkaichi Budokai Ring Out
			{
				ram.write<s32>(offset.battleIsPaused, 0);
			}
		}
	}

	void Misc::enable(bool enable)
	{
		m_isEnabled = enable;
		m_timescale = timescaleBase(m_game->version());

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
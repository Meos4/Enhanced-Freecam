#include "Misc.hpp"

#include "Common/Buffer.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::DBZInfiniteWorld
{
	static constexpr auto timescaleMin{ 0.1f }, timescaleMax{ 3.f };

	static float timescaleBase(s32 version)
	{
		return version == Version::Pal ? 1.2f : 1.f;
	}

	Misc::Misc(Game* game)
		: m_game(game), m_timescale(timescaleBase(game->version()))
	{
	}

	void Misc::draw()
	{
		const std::array<Ui::LabelFlag, 2> lf
		{
			"Pause Game", &m_isGamePaused,
			"Hide Hud", &m_isHudHidden
		};

		const auto state{ m_game->state() };

		if (state != State::DragonMissionCutscene && state != State::DragonMissionFlying)
		{
			const auto base{ timescaleBase(m_game->version()) };
			MiscModel::drawTimescale(&m_timescale, timescaleMin * base, timescaleMax * base, !m_isEnabled);
		}

		MiscModel::drawFlags("Misc", lf, !m_isEnabled);
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

			MiscModel::increaseTimescale(input, Input::TimescalePos, &m_timescale, timescaleMin * base, timescaleMax * base);
			MiscModel::decreaseTimescale(input, Input::TimescaleNeg, &m_timescale, timescaleMin * base, timescaleMax * base);

			if (state == State::DragonMissionCutscene || state == State::DragonMissionFlying)
			{
				m_timescale = base;
			}
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		Mips_t
			m013ubInstr,
			m014ubInstr,
			m019uInstr;

		if (version == Version::NtscU)
		{
			m013ubInstr = 0x27BDFF00;
			m014ubInstr = 0x27BDFF00;
			m019uInstr = 0x27BDFF90;
		}
		else
		{
			m013ubInstr = 0x27BDFEF0;
			m014ubInstr = 0x27BDFEF0;
			m019uInstr = 0x27BDFFA0;
		}

		const bool isPauseMenuEnabled{ ram.read<s16>(offset.battleIsPauseMenuEnabled) == 1 };

		if (state == State::Battle)
		{
			ram.write(offset.battleTimescale, m_timescale);

			if (m_isGamePaused && !isPauseMenuEnabled)
			{
				ram.write<s16>(offset.battleIsPauseMenuEnabled + 0x14, 0); // Type of menu
				ram.write<s32>(offset.battleIsPaused, 1);

				// Prevent "A supported controller is not connected in controller port 1..."
				SBuffer<0xBC> buffer;
				ram.read(offset.padData, buffer.data(), buffer.size());
				ram.write(offset.battlePadData, buffer);
			}
			else if (ram.read<s32>(offset.battleIsPaused + 0x10) == 4) // Tenkaichi Budokai Ring Out
			{
				ram.write<s32>(offset.battleIsPaused, 0);
			}
		}
		else if (state == State::DragonMission)
		{
			ram.write(offset.battleTimescale - 8, m_timescale);
		}

		ram.write(offset.Fn_battleUpdatePauseMenu + 0x66C, state == State::Battle && m_isGamePaused && !isPauseMenuEnabled ? 0x00000000 : 0xAC608B7C);
		ram.writeConditional(state == State::Battle && m_isHudHidden,
			offset.Fn_battleShouldDrawHud + 0x38, 0x00001021, 0x00000000,
			offset.Fn_battleDrawDuelingInfo + 0x58, 0x10000029, 0x10600029
		);

		ram.writeConditional((state != State::Battle && state != State::None) && m_isHudHidden,
			offset.Fn_mdDrawHud, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0xFFBF0040 },
			offset.Fn_drawSprites + 0x44, 0x10000018, 0x18400018,
			offset.Fn_MSNF_FIX_DRAW, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0040 },
			offset.Fn_FLIGHT_draw + 0x160, 0x10000030, 0x50400030 // Arrow
		);

		ram.writeConditional((state != State::Battle && state != State::None) && m_isGamePaused,
			offset.Fn_mdCutsceneNextStep + 0x250, 0x00000000, 0xE6000168, // Timer
			offset.Fn_MSN006_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN006_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN006_Upd, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x24050001 },
			offset.Fn_MSNF_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF70, 0xFFBF0080 },
			offset.Fn_MSNF_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF20, 0xFFBF00B0 },
			offset.Fn_MSN007_Mai, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x24060002 },
			offset.Fn_MSN007_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			offset.Fn_MSN007_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			offset.Fn_MSN010_PLY_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0xFFBF0050 },
			offset.Fn_MSN010_PLY_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			offset.Fn_mdKrilin_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF20, 0xFFBF0070 },
			offset.Fn_mdKrilin_PLY_PRE_EXEC + 0x24, 0x10000014, 0x18600014,
			offset.Fn_MSN014_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN014_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN014_updateBars, Mips::jrRaNop(), std::array<Mips_t, 2>{ m014ubInstr, 0x24030001 },
			offset.Fn_MSN012_Mai, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0x24040002 },
			offset.Fn_MSN012_Upd, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF90, 0x24040001 },
			offset.Fn_MSN012_PRE_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			offset.Fn_MSN012_POST_EXE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			offset.Fn_mdTenshinhan_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			offset.Fn_mdTenshinhan_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 },
			offset.Fn_MSN013_NPC_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN013_PLY_PRE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0xFFBF0000 },
			offset.Fn_MSN013_Upd + 0xD4, 0x1000002E, 0x5443002E, // Bars
			offset.Fn_MSN013_Upd + 0x198, 0x1000002D, 0x5444002D,
			offset.Fn_MSN013_updateLongBars, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFEA0, 0x24030001 },
			offset.Fn_MSN019_PLY_PRE_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC0, 0xFFBF0030 },
			offset.Fn_MSN019_PLY_POST_EXEC, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0040 },
			offset.Fn_MSN019_UPDATE, Mips::jrRaNop(), std::array<Mips_t, 2>{ m019uInstr, 0x3C02004B },
			offset.Fn_FLIGHT_PLAYER, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0030 },
			offset.Fn_FLIGHT_UPDATE, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFA0, 0xFFBF0050 },
			offset.Fn_FLIGHT_nextStep + 0x4C, 0x00000000, 0xE680000C, // Timer
			offset.Fn_FLIGHT_nextStep + 0xEC, 0x10000090, 0x12400090, // Next Step
			offset.Fn_SHOOT_update, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 }
		);
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
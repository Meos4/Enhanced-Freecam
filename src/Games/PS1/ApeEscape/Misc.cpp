#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS1::ApeEscape
{
	Misc::Misc(Game* game)
		: m_game(game)
	{
	}

	void Misc::draw()
	{
		if (m_game->state() == State::Ingame)
		{
			const std::array<Ui::LabelFlag, 2> lf
			{
				"Pause Game", &m_isGamePaused,
				"Hide Hud", &m_isHudHidden
			};

			MiscModel::drawFlags("Misc", lf, !m_isEnabled);
		}
		else
		{
			const Ui::LabelFlag lf{ "Hide Hud", &m_isHudHidden };
			MiscModel::drawFlags("Misc", { &lf, 1 }, !m_isEnabled);
		}
	}

	void Misc::update()
	{
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

			if (state != State::Ingame)
			{
				m_isGamePaused = false;
			}
		}

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto version{ m_game->version() };

		u32 iglShift,
			csShift,
			ssShift,
			ssShift2,
			smShift,
			smShift2,
			spShift,
			spShift2,
			skrShift,
			skrShift2,
			sbShift,
			sbShift2,
			sbShift3;

		Mips_t 
			spInstr,
			skrInstr,
			skrInstr2,
			sbInstr;

		if (version == Version::NtscU)
		{
			iglShift = 0x198;
			csShift = 0x1DC4;
			ssShift = 0xFA8;
			ssShift2 = 0x1280;
			smShift = 0x3CF0;
			smShift2 = 0x9B30;
			spShift = 0x32F4;
			spShift2 = 0x3594;
			skrShift = 0x2EEC;
			skrShift2 = 0x228C8;
			sbShift = 0x39EC;
			sbShift2 = 0x3D7C;
			sbShift3 = 0xEA34;
			spInstr = 0x27BDFFD0;
			skrInstr = 0x0C049FA6;
			skrInstr2 = 0x0C0427E2;
			sbInstr = 0x0C047081;
		}
		else
		{
			iglShift = 0x164;
			csShift = 0x1E94;
			ssShift = 0xF74;
			ssShift2 = 0x1244;
			smShift = 0x3CF4;
			smShift2 = 0x9B40;
			spShift = 0x2DE0;
			spShift2 = 0x369C;
			skrShift = 0x2E44;
			skrShift2 = 0x22834;
			sbShift = 0x3B1C;
			sbShift2 = 0x3D5C;
			sbShift3 = 0xE944;
			spInstr = 0x27BDFFC8;
			skrInstr = 0x0C049F81;
			skrInstr2 = 0x0C0427B8;
			sbInstr = 0x0C047045;
		}

		if (version == Version::NtscJRev1)
		{
			smShift = 0x3D18;
			smShift2 = 0x9B70;
		}

		ram.writeConditional(m_isGamePaused,
			offset.Fn_inGameLoop + iglShift, 0x2402FFFF, 0x8E020100,
			offset.Fn_inGameLoop + iglShift + 0x150, 0x2402FFFF, 0x8E020100,
			offset.Fn_updateAdditional + 0x30, 0x2402FFFF, 0x8C420100,
			offset.Fn_updateMonkey + 0x154, 0x2402FFFF, 0x8E020100,
			offset.Fn_updateCollectable + 0x18, 0x2402FFFF, 0x8C420100,
			offset.Fn_updateMovable + 0x34, 0x2402FFFF, 0x8C620100
		);

		ram.write(offset.Fn_inGameLoop + iglShift + 0x34C, m_isHudHidden ? 0x00000000 : 0x10400007);

		if (version == Version::NtscJ || version == Version::NtscJRev1)
		{
			// Subtitles
			ram.write(offset.Fn_drawText, m_isHudHidden ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFF0, 0xAFB20008 });
		}

		if (state == State::TitleScreen)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + 0x1F8, 0x3C060400, 0x3C0600C8, // SCE
				offset.overlay + 0x26C, 0x34C60400, 0x34C60040, // Saru Get You Logo
				offset.overlay + 0xC6C, 0x1000001F, 0x1040001F // Cursor
			);
		}
		else if (state == State::StageSelect)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + ssShift, 0x24020400, 0x240200CA, // Buttons Text
				offset.overlay + ssShift + 0x90, 0x24020400, 0x240200BC, // Buttons
				offset.overlay + ssShift2, 0x240C0400, 0x240C00C0, // LR Greenlines
				offset.overlay + ssShift2 + 4, 0x240B0400, 0x240B00C1,
				offset.overlay + ssShift2 + 0x15E8, 0x24020400, 0x00A01021 // return str middle shift
			);
		}
		else if (state == State::SpaceMenu)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + smShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF8, 0x3C07800F },
				offset.overlay + smShift2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xAFB00010 }
			);
		}
		else if (state == State::StagePreview)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + spShift, 0x00000000, 0x10600007, // Map info
				offset.overlay + spShift2, 0x3C050000, 0x3C050020, // WARNING! Y Size
				offset.overlay + 0x25B0, Mips::jrRaNop(), std::array<Mips_t, 2>{ spInstr, 0x3C028014 }
			);
		}
		else if (state == State::ClearStage)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + 0x12D0, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00804821, 0x3C02800F },
				offset.overlay + csShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x00003821 },
				offset.overlay + csShift + 0x5A8, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C02800F }
			);
		}
		else if (state == State::RaceResult)
		{
			ram.write(offset.overlay + 0x1D8, m_isHudHidden ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x18A00009, 0x00001821 } );
		}
		else if (state == State::SkiKidzRacing)
		{
			ram.writeConditional(m_isHudHidden,
				offset.minigame + skrShift, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0x12C, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0x240, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0x6FC, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0xB68, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0x7414, 0x00000000, skrInstr,
				offset.minigame + skrShift + 0xD9A4, 0x00000000, skrInstr + 0x10, // Hide Char
				offset.minigame + skrShift + 0x130A0, 0x00000000, skrInstr,
				offset.minigame + skrShift2, 0x00000000, skrInstr2
			);
		}
		else if (state == State::SpecterBoxing)
		{
			ram.writeConditional(m_isHudHidden,
				offset.minigame + 0x1D1C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0xAFB20020 },
				offset.minigame + 0x1FE0, 0x00000000, sbInstr, 
				offset.minigame + 0x2428, 0x00000000, sbInstr,
				offset.minigame + sbShift, 0x00000000, sbInstr,
				offset.minigame + sbShift2, 0x00000000, sbInstr,
				offset.minigame + sbShift2 + 0xC4, 0x00000000, sbInstr,
				offset.minigame + sbShift2 + 0xE0, 0x00000000, sbInstr,
				offset.minigame + sbShift2 + 0xFC, 0x00000000, sbInstr,
				offset.minigame + sbShift2 + 0x118, 0x00000000, sbInstr,
				offset.minigame + sbShift2 + 0x2CC, 0x00000000, sbInstr,
				offset.minigame + sbShift3, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x38, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x74, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0xA8, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0xD0, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0xF8, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x120, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x148, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x1E8, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x290, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x338, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x3E0, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x6440, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x64AC, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x650C, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x6564, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x65C0, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x6650, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x66D0, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x6704, 0x00000000, sbInstr,
				offset.minigame + sbShift3 + 0x673C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC8, 0xAFB00018 }
			);
		}
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
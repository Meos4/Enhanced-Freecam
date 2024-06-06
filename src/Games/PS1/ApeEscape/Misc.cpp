#include "Misc.hpp"

#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"

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
			MiscModel::drawMiscPauseGameHideHud(&m_isGamePaused, &m_isHudHidden, !m_isEnabled);
		}
		else
		{
			MiscModel::drawMiscHideHud(&m_isHudHidden, !m_isEnabled);
		}
	}

	void Misc::update()
	{
		const auto state{ m_game->state() };

		if (m_isEnabled)
		{
			auto* const input{ m_game->input() };
			MiscModel::toggle(input, Input::HideHud, &m_isHudHidden);

			if (state == State::Ingame)
			{
				MiscModel::toggle(input, Input::PauseGame, &m_isGamePaused);
			}
			else
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
			sbShift3,
			gmShift,
			gmShift2;

		Mips_t 
			skrInstr,
			skrInstr2,
			sbInstr,
			gmInstr;

		if (version == Version::NtscU)
		{
			iglShift = 0x198;
			csShift = 0x1DC4;
			ssShift = 0xFA8;
			ssShift2 = 0x1280;
			smShift = 0x3CF0;
			smShift2 = 0x9B30;
			spShift = 0x3248;
			spShift2 = 0x34D0;
			skrShift = 0x2EEC;
			skrShift2 = 0x228C8;
			sbShift = 0x39EC;
			sbShift2 = 0x3D7C;
			sbShift3 = 0xEA34;
			gmShift = 0x2FD4;
			gmShift2 = 0x7AF0;
			skrInstr = 0x0C049FA6;
			skrInstr2 = 0x0C0427E2;
			sbInstr = 0x0C047081;
			gmInstr = 0x90C80000;
		}
		else
		{
			iglShift = 0x164;
			csShift = 0x1E94;
			ssShift = 0xF74;
			ssShift2 = 0x1244;
			smShift = 0x3CF4;
			smShift2 = 0x9B40;
			spShift = 0x3318;
			spShift2 = 0x35D8;
			skrShift = 0x2E44;
			skrShift2 = 0x22834;
			sbShift = 0x3B1C;
			sbShift2 = 0x3D5C;
			sbShift3 = 0xE944;
			gmShift = 0x2FD4;
			gmShift2 = 0x7B20;
			skrInstr = 0x0C049F81;
			skrInstr2 = 0x0C0427B8;
			sbInstr = 0x0C047045;
			gmInstr = 0x90C80000;
		}

		if (version == Version::NtscJRev1)
		{
			smShift = 0x3D18;
			smShift2 = 0x9B70;
			gmShift = 0x2FDC;
			gmShift2 = 0x7B28;
			gmInstr = 0x90C30000;
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
				offset.overlay + ssShift2, 
					std::array<Mips_t,2>{ 0x240C0400, 0x240B0400 }, std::array<Mips_t,2>{ 0x240C00C0, 0x240B00C1 }, // LR Greenlines
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
				offset.overlay + spShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD8, 0x3C02800F }, // Map info
				offset.overlay + spShift2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C03001E, 0x3C02800F }, // Stadium Attack
				offset.overlay + spShift2 + 0xC4, 0x3C050000, 0x3C050020 // WARNING! Y Size
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
		else if (state == State::GalaxyMonkey)
		{
			ram.writeConditional(m_isHudHidden,
				offset.overlay + 0x1048, 0x00001021, 0x24020100, // Logo Y Size
				offset.overlay + 0x2368, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFF0, 0x00805021 }, // Text
				offset.overlay + 0x24D4, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x8C4A0004, gmInstr }, // Mid Text
				offset.overlay + gmShift, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C085555, 0x35085555 }, // Green Frame
				offset.overlay + gmShift + 0x2470, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x3C07E100, 0x34E7020F }, // Life
				offset.overlay + gmShift + 0x3AC0, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00005821, 0x3C028014 }, // Large Text
				offset.overlay + gmShift + 0x421C, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFC8, 0x3C04800F },
				offset.overlay + gmShift2, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xAFB00010 } // Text
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
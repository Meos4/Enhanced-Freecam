#include "Misc.hpp"

#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::HauntingGround
{
	Misc::Misc(Game* game)
		: m_game(game)
	{
	}

	void Misc::draw()
	{
		const Ui::LabelFlag lf{ "Pause Game", &m_isGamePaused };
		MiscModel::drawFlags("Misc", { &lf, 1}, !m_isEnabled);
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

		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto uiShift{ m_game->version() == Version::Pal ? 0xD94 : 0xD8C };

		ram.writeConditional(m_isGamePaused,
			offset.Fn_updateIngame + uiShift, 0x1000004F, 0x3C0200F7,
			offset.Fn_isParticleShouldntBeAnimated + 0x34, 0x24020001, 0x00000000,
			offset.Fn_updatePanic + 0x54, 0x1000000B, 0x1440000B
		);

		const bool isAdxRunning{ ram.read<u8>(offset.adxFlags) == 1 };
		const auto sofdecVideoStructPtr{ ram.read<u32>(offset.sofdecVideoStructPtr) };
		const bool isInCutscene{ sofdecVideoStructPtr != 0 };
		const bool isCutscenePaused{ sofdecVideoStructPtr ? (ram.read<s32>(sofdecVideoStructPtr + 0x50) == 1 ? true : false) : false };

		if (isInCutscene && !isCutscenePaused)
		{
			if (m_isGamePaused && isAdxRunning)
			{
				ram.write(offset.adxFlags, u8(0)); // Pause
				ram.write(offset.Fn_isSofdecVideoRunning + 0x30, 0x00001021);
			}
			else if (!m_isGamePaused && !isAdxRunning)
			{
				ram.write(offset.adxFlags, u8(1)); // Resume
				ram.write(offset.Fn_isSofdecVideoRunning + 0x30, 0x00000000);
			}
		}
	}

	void Misc::enable(bool enable)
	{
		m_isEnabled = enable;

		if (enable)
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
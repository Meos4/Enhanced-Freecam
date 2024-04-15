#include "Game.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS2::DBZTenkaichi3
{
	Game::Game(Ram ram, s32 version)
		: m_ram(ram), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-54945]",
			"NTSC-U - [SLUS-21678]",
			"NTSC-J - [SLPS-25815]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x002F45A0, { 92, 83, 76, 69, 83, 95, 53, 51, 50, 46, 48, 48, 59, 49, 0, 0, 160, 85, 38, 0, 168, 85, 38, 0, 176, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 184, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 208, 85, 38, 0, 192, 85, 38, 0, 208, 85, 38, 0 },
			0x002F34F0, { 92, 83, 76, 85, 83, 95, 50, 49, 50, 46, 50, 55, 59, 49, 0, 0, 0, 79, 38, 0, 8, 79, 38, 0, 16, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 24, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 48, 79, 38, 0, 32, 79, 38, 0, 48, 79, 38, 0 },
			0x002F34F0, { 92, 83, 76, 80, 83, 95, 50, 53, 53, 46, 54, 48, 59, 49, 0, 0, 16, 77, 38, 0, 24, 77, 38, 0, 32, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 40, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 64, 77, 38, 0, 48, 77, 38, 0, 64, 77, 38, 0 }

		};

		return { vOp[version].offset, vOp[version].pattern };
	}

	std::unique_ptr<GameLoop> Game::createLoop(Ram ram, s32 version)
	{
		return std::make_unique<Loop>(Game{ ram, version });
	}

	std::vector<InputWrapper::NameInputs> Game::baseInputs()
	{
		const auto& i{ g_settings.input };

		return
		{
			{ "Toggle Freecam", i.toggleFreecam },
			{ "Move Forward", i.moveForward },
			{ "Move Backward", i.moveBackward },
			{ "Move Right", i.moveRight },
			{ "Move Left", i.moveLeft },
			{ "Move Up", i.moveUp },
			{ "Move Down", i.moveDown },
			{ "Rotate X+", i.rotateUp },
			{ "Rotate X-", i.rotateDown },
			{ "Rotate Y+", i.rotateRight },
			{ "Rotate Y-", i.rotateLeft },
			{ "Rotate Z+", i.rotateClockwise },
			{ "Rotate Z-", i.rotateCounterclockwise },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud }
		};
	}

	std::span<const char* const> Game::stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Battle",
			"Battle Cutscene",
			"Dragon History",
			"View Model",
			"Shenron Cutscene",
			"Shenron Wish"
		};

		return names;
	}

	const PCSX2::PnachInfo& Game::pnachInfo() const
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"A422BB13", 0x00100000, 0x002C067C,
			"428113C2", 0x00100000, 0x002BF6AC,
			"F28D21F1", 0x00100000, 0x002BF5A4
		};

		return pnachInfos[m_version];
	}

	void Game::update()
	{
		const auto cutscenePtr{ m_ram.read<u32>(m_offset.battlePlayerPtr + 0xB8) };

		if (PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.battlePlayerPtr)))
		{
			if (m_ram.read<s32>(m_offset.splitscreen) == 0)
			{
				m_state = State::Battle;

				if (PS2::isValidMemoryRange(cutscenePtr))
				{
					if (m_ram.read<s32>(cutscenePtr + 0x32C) == 1)
					{
						m_state = State::DragonHistory;
					}
					else if (PS2::isValidMemoryRange(m_ram.read<u32>(cutscenePtr + 0x2C0)))
					{
						m_state = State::BattleCutscene;
					}
				}
			}
			else 
			{
				m_state = State::None;
			}
		}
		else if (const auto viewModelPtr{ m_ram.read<u32>(m_offset.viewModelPtr) };
				PS2::isValidMemoryRange(viewModelPtr) && m_ram.read<s32>(viewModelPtr + 0x1628) == 7)
		{
			m_state = State::ViewModel;
		}
		else if (PS2::isValidMemoryRange(cutscenePtr))
		{
			m_state = m_ram.read<s32>(cutscenePtr + 0x32C) == 1 ? State::ShenronWish : State::ShenronCutscene;
		}
		else
		{
			m_state = State::None;
		}
	}

	void Game::readSettings(const Json::Read& json)
	{
		m_settings.readSettings(json);
		m_input.readSettings(json);
	}

	void Game::writeSettings(Json::Write* json)
	{
		m_settings.writeSettings(json);
		m_input.writeSettings(json);
	}

	const Ram& Game::ram() const
	{
		return m_ram;
	}

	s32 Game::version() const
	{
		return m_version;
	}

	const Offset& Game::offset() const
	{
		return m_offset;
	}

	Settings* Game::settings()
	{
		return &m_settings;
	}

	InputWrapper* Game::input()
	{
		return &m_input;
	}

	s32 Game::state() const
	{
		return m_state;
	}
}
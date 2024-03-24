#include "Game.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS2::DragonQuest8
{
	Game::Game(Ram ram, s32 version)
		: m_ram(ram), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-53974]",
			"NTSC-U - [SLUS-21207]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x003A5411, { 79, 242, 111, 243, 111, 244, 111, 245, 111, 246, 111, 217, 85, 218, 85, 219, 85, 220, 85, 249, 117, 250, 117, 251, 117, 252, 117, 223, 115, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 51, 57, 55, 52, 100, 113, 56, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
			0x003990A1, { 79, 242, 111, 243, 111, 244, 111, 245, 111, 246, 111, 217, 85, 218, 85, 219, 85, 220, 85, 249, 117, 250, 117, 251, 117, 252, 117, 223, 115, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 50, 48, 55, 100, 113, 56, 63, 63, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
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
			{ "Rotate Y+", i.rotateLeft },
			{ "Rotate Y-", i.rotateRight },
			{ "Rotate Z+", i.rotateCounterclockwise },
			{ "Rotate Z-", i.rotateClockwise },
			{ "Fov+", i.fovIncrease },
			{ "Fov-", i.fovDecrease },
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
			"Common",
			"Cutscene",
			"Companions",
			"Alchemy / Mini map",
			"Battle"
		};

		return names;
	}

	void Game::update()
	{
		switch (m_ram.read<s32>(m_offset.gameState))
		{
		case 0: m_state = State::Common; return;
		case 1: m_state = State::Cutscene; return;
		case 3: m_state = State::Companions; return;
		case 6: m_state = State::Alchemy_Minimap; return;
		case 7: m_state = State::Battle; return;
		}

		m_state = State::None;
	}

	const PCSX2::PnachInfo& Game::pnachInfo() const
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"945FBF31", 0x00100000, 0x0038DA08,
			"F4715852", 0x00100000, 0x0038183C
		};

		return pnachInfos[m_version];
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
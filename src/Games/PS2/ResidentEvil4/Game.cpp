#include "Game.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	Game::Game(Ram ram, s32 version)
		: m_ram(ram), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"NTSC-J - [SLPM-66213]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x003AF2B8, { 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 172, 162, 22, 0, 88, 162, 22, 0, 0, 0, 0, 0, 66, 73, 83, 76, 80, 77, 45, 54, 54, 50, 49, 51, 83, 89, 83, 0, 98, 105, 111, 52, 46, 105, 99, 111 }
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
			{ "Pause Game", i.pauseGame },
			{ "Hide Hud", i.hideHud },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Fov Speed+", i.fovSpeedPos },
			{ "Fov Speed-", i.fovSpeedNeg }
		};
	}

	const PCSX2::PnachInfo& Game::pnachInfo() const
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"60FA8C69", 0x00100000, 0x002F0518
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
}
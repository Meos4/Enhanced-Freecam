#include "Game.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS2::HauntingGround
{
	Game::Game(Ram ram, s32 version)
		: m_ram(ram), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-52877]",
			"NTSC-U - [SLUS-21075]",
			"NTSC-J - [SLPM-65913]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00463A52, { 83, 76, 69, 83, 45, 53, 50, 56, 55, 55, 72, 71, 0, 0, 37, 100, 47, 37, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 48, 50, 88, 47, 37, 48, 50, 88, 47, 50, 48, 37, 48, 50, 88, 32, 37, 48, 50, 88, 58, 37, 48, 50, 88, 0, 0, 0, 0, 0, 0, 32, 120 },
			0x00463A52, { 83, 76, 85, 83, 45, 50, 49, 48, 55, 53, 72, 71, 0, 0, 37, 100, 47, 37, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 85, 66, 83, 67, 82, 92, 77, 83, 71, 95, 83, 85, 66, 46, 66, 73, 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 37, 48 },
			0x00463B22, { 83, 76, 80, 77, 45, 54, 53, 57, 49, 51, 68, 69, 77, 69, 78, 84, 79, 0, 0, 0, 0, 0, 37, 100, 47, 37, 100, 0, 0, 0, 83, 85, 66, 83, 67, 82, 92, 77, 83, 71, 95, 83, 85, 66, 46, 66, 73, 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 85 }
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
			{ "Rotate X+", i.rotateDown },
			{ "Rotate X-", i.rotateUp },
			{ "Rotate Y+", i.rotateLeft },
			{ "Rotate Y-", i.rotateRight },
			{ "Rotate Z+", i.rotateCounterclockwise },
			{ "Rotate Z-", i.rotateClockwise },
			{ "Fov+", i.fovIncrease },
			{ "Fov-", i.fovDecrease },
			{ "Pause Game", i.pauseGame },
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
			"2CD5794C", 0x00100000, 0x003A3D00,
			"901AAC09", 0x00100000, 0x003A1B7C,
			"E263BC4B", 0x00100000, 0x003A1BC0
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
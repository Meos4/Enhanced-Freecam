#include "Game.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS1::DinoCrisis
{
	Game::Game(Ram&& ram, s32 version)
		: m_ram(std::move(ram)), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"NTSC-U - [SLUS-00922]",
			"NTSC-U v1.1 - [SLUS-00922]",
			"NTSC-J - [SLPS-02180]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00018448, { 32, 115, 2, 128, 212, 115, 2, 128, 68, 116, 2, 128, 120, 116, 2, 128, 60, 117, 2, 128, 92, 118, 2, 128, 32, 119, 2, 128, 184, 119, 2, 128, 180, 120, 2, 128, 68, 121, 2, 128, 132, 154, 2, 128, 216, 154, 2, 128, 84, 155, 2, 128, 252, 155, 2, 128, 252, 155, 2, 128, 252, 155, 2, 128 },
			0x00018448, { 28, 115, 2, 128, 208, 115, 2, 128, 64, 116, 2, 128, 116, 116, 2, 128, 56, 117, 2, 128, 88, 118, 2, 128, 28, 119, 2, 128, 180, 119, 2, 128, 176, 120, 2, 128, 64, 121, 2, 128, 128, 154, 2, 128, 212, 154, 2, 128, 80, 155, 2, 128, 248, 155, 2, 128, 248, 155, 2, 128, 248, 155, 2, 128 },
			0x00010478, { 16, 242, 1, 128, 196, 242, 1, 128, 52, 243, 1, 128, 104, 243, 1, 128, 44, 244, 1, 128, 76, 245, 1, 128, 16, 246, 1, 128, 168, 246, 1, 128, 164, 247, 1, 128, 52, 248, 1, 128, 116, 25, 2, 128, 200, 25, 2, 128, 68, 26, 2, 128, 236, 26, 2, 128, 236, 26, 2, 128, 236, 26, 2, 128 }
		};

		return { vOp[version].offset, vOp[version].pattern };
	}

	std::unique_ptr<GameLoop> Game::createLoop(Ram&& ram, s32 version)
	{
		return std::make_unique<Loop>(Game{ std::move(ram), version });
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
			{ "Fov+", i.fovDecrease },
			{ "Fov-", i.fovIncrease },
			{ "Pause Game", i.pauseGame },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Fov Speed+", i.fovSpeedPos },
			{ "Fov Speed-", i.fovSpeedNeg }
		};
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
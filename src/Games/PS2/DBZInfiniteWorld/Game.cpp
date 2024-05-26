#include "Game.hpp"

#include "Common/PS2/PS2.hpp"

#include "Common/Settings.hpp"

#include "Loop.hpp"

#include <array>

namespace PS2::DBZInfiniteWorld
{
	Game::Game(Ram&& ram, s32 version)
		: m_ram(std::move(ram)), m_version(version), m_offset(Offset::create(version)), m_input(&Game::baseInputs)
	{
	}

	const char* Game::versionText(s32 version)
	{
		static constexpr std::array<const char*, Version::Count> vText
		{
			"PAL - [SLES-55347]",
			"NTSC-U - [SLUS-21842]",
			"NTSC-J - [SLPS-25905]"
		};

		return vText[version];
	}

	OffsetPattern Game::offsetPattern(s32 version)
	{
		static constexpr std::array<OffsetPatternStatic<u32, 64>, Version::Count> vOp
		{
			0x00497B73, { 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 47, 66, 69, 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 69, 83, 76, 69, 83, 45, 53, 53, 51, 52, 55, 68, 66, 90, 73, 87, 47 },
			0x00497373, { 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 65, 83, 76, 85, 83, 45, 50, 49, 56, 52, 50, 68, 66, 90, 73, 87, 47 },
			0x00497AD3, { 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 47, 66, 73, 83, 76, 80, 83, 45, 50, 53, 57, 48, 53, 68, 66, 90, 73, 87, 47 }
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
			{ "Timescale+", i.timescalePos },
			{ "Timescale-", i.timescaleNeg },
			{ "Movement Speed+", i.movementSpeedPos },
			{ "Movement Speed-", i.movementSpeedNeg },
			{ "Rotation Speed+", i.rotationSpeedPos },
			{ "Rotation Speed-", i.rotationSpeedNeg },
			{ "Fov Speed+", i.fovSpeedPos },
			{ "Fov Speed-", i.fovSpeedNeg }
		};
	}

	std::span<const char* const> Game::stateNames()
	{
		static constexpr std::array<const char*, State::Count> names
		{
			"Battle",
			"Dragon Mission",
			"Dragon Mission (Flying)",
			"Dragon Mission (Cutscene)"
		};

		return names;
	}

	const PCSX2::PnachInfo& Game::pnachInfo() const
	{
		static constexpr std::array<PCSX2::PnachInfo, Version::Count> pnachInfos
		{
			"335A5A1F", 0x00100000, 0x003826EC,
			"E90DC5C9", 0x00100000, 0x00381FC4,
			"C7E583CF", 0x00100000, 0x00382614
		};

		return pnachInfos[m_version];
	}

	void Game::update()
	{
		const auto battleCameraPtr{ m_ram.read<u32>(m_offset.battleCameraPtr) };

		if (PS2::isValidMemoryRange(battleCameraPtr))
		{
			if (PS2::isValidMemoryRange(m_ram.read<u32>(battleCameraPtr + 0x240)))
			{
				m_state = State::Battle;
				return;
			}
			else if (PS2::isValidMemoryRange(m_ram.read<u32>(battleCameraPtr + 0x244)))
			{
				m_state = State::DragonMission;
				return;
			}
		}

		const auto flightCameraPtr{ m_ram.read<u32>(m_offset.FLIGHT_cameraPtr) };

		if (PS2::isValidMemoryRange(flightCameraPtr) && m_ram.read<float>(flightCameraPtr + 0x64C) == 1.f)
		{
			m_state = State::DragonMissionFlying;
			return;
		}

		if (PS2::isValidMemoryRange(m_ram.read<u32>(m_offset.FLIGHT_cameraPtr + 0x70)))
		{
			m_state = State::DragonMissionCutscene;
			return;
		}

		m_state = State::None;
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
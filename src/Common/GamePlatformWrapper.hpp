#pragma once

#include "AsyncGameSearcher.hpp"
#include "GameInfo.hpp"
#include "Process.hpp"

#include <memory>
#include <span>

class GamePlatformWrapper final
{
public:
	GamePlatformWrapper(s32 platform);

	const char* platformName() const;
	const char* gameName() const;
	const char* versionName() const;
	const char* settingsName() const;
	std::span<const GameInfo> gamesInfo() const;
	const GameInfo& gameInfo() const;
	std::span<const EmulatorInfo> emulators() const;
	bool isAnEmulator() const;
	AGSCallback agsCallback() const;
	Ram createRam(std::shared_ptr<Process> process, std::uintptr_t begin) const;

	s32 platform() const;
	s32 game() const;
	s32 version() const;

	void setPlatform(s32 platform);
	void setGame(s32 game);
	void setVersion(s32 version);
private:
	s32 m_platform;
	s32 m_game{};
	s32 m_version{};
};
#include "GamePlatformWrapper.hpp"

#include "PS1/PS1.hpp"
#include "PS2/PS2.hpp"

#include "EfException.hpp"
#include "GamePlatform.hpp"

GamePlatformWrapper::GamePlatformWrapper(s32 platform)
	: m_platform(platform)
{
}

const char* GamePlatformWrapper::platformName() const
{
	return GamePlatform::name(m_platform);
}

const char* GamePlatformWrapper::gameName() const
{
	return this->gameInfo().name;
}

const char* GamePlatformWrapper::versionName() const
{
	return this->gameInfo().versionText(m_version);
}

const char* GamePlatformWrapper::settingsName() const
{
	switch (m_platform)
	{
	case GamePlatform::PS1: return PS1::settingsName;
	case GamePlatform::PS2: return PS2::settingsName;
	default: throw EfException{ "Wrong platform: {}", m_platform };
	}
}

std::span<const GameInfo> GamePlatformWrapper::gamesInfo() const
{
	return GamePlatform::gamesInfo(m_platform);
}

const GameInfo& GamePlatformWrapper::gameInfo() const
{
	return this->gamesInfo()[m_game];
}

std::span<const EmulatorInfo> GamePlatformWrapper::emulators() const
{
	switch(m_platform)
	{
	case GamePlatform::PS1: return PS1::emulators();
	case GamePlatform::PS2: return PS2::emulators();
	default: throw EfException{ "Wrong platform: {}", m_platform };
	}
}

bool GamePlatformWrapper::isAnEmulator() const
{
	return true;
}

AGSCallback GamePlatformWrapper::agsCallback() const
{
	switch (m_platform)
	{
	case GamePlatform::PS1: return PS1::asyncGameSearcher;
	case GamePlatform::PS2: return PS2::asyncGameSearcher;
	default: throw EfException{ "Wrong platform: {}", m_platform };
	}
}

Ram GamePlatformWrapper::createRam(std::shared_ptr<Process> process, std::uintptr_t begin) const
{
	switch (m_platform)
	{
	case GamePlatform::PS1: return PS1::createRam(process, begin);
	case GamePlatform::PS2: return PS2::createRam(process, begin);
	default: throw EfException{ "Wrong platform: {}", m_platform };
	}
}

s32 GamePlatformWrapper::platform() const
{
	return m_platform;
}

s32 GamePlatformWrapper::game() const
{
	return m_game;
}

s32 GamePlatformWrapper::version() const
{
	return m_version;
}

void GamePlatformWrapper::setPlatform(s32 platform)
{
	if (m_platform != platform)
	{
		m_platform = platform;
		m_game = 0;
		m_version = 0;
	}
}

void GamePlatformWrapper::setGame(s32 game)
{
	if (m_game != game)
	{
		m_game = game;
		m_version = 0;
	}
}

void GamePlatformWrapper::setVersion(s32 version)
{
	m_version = version;
}
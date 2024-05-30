#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/Json.hpp"

namespace PS2::DragonQuest8
{
	Loop::Loop(Game&& game)
		: m_game(std::move(game)), m_freecam(&m_game), m_bonus(&m_game)
	{
		const auto jsonRead{ Json::read(PS2::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			const auto& json{ jsonRead.value() };
			m_game.readSettings(json);
			m_bonus.readSettings(json);
		}
	}

	Loop::~Loop()
	{
		Json::Write json;
		m_game.writeSettings(&json);
		m_bonus.writeSettings(&json);
		Json::overwrite(json, PS2::settingsFilePath(Game::name));

		if (isValid())
		{
			m_freecam.enable(false);
			m_freecam.update();
		}
	}

	void Loop::draw()
	{
		DRAW_GAME_WINDOWS(m_freecam.draw(), m_game.input()->draw(), m_game.settings()->draw(), m_bonus.draw());
		PS2_PCSX2_DRAW_PNACH_BEHAVIOR_WINDOW;
		PS2_DEBUG_DRAW_WINDOW;
	}

	void Loop::update()
	{
		m_game.update();
		m_freecam.update();
		m_bonus.update();
	}

	bool Loop::isValid()
	{
		const auto [offset, pattern]{ Game::offsetPattern(m_game.version()) };
		return m_game.ram().isPatternValid(offset, pattern);
	}
}
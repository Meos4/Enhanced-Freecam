#include "Loop.hpp"

#include "Common/PS1/Debug.hpp"
#include "Common/PS1/PS1.hpp"

#include "Common/Console.hpp"
#include "Common/Json.hpp"

#include "CustomCode.hpp"

namespace PS1::ApeEscape
{
	Loop::Loop(Game game)
		: m_game(game), m_freecam(&m_game), m_bonus(&m_game)
	{
		const auto jsonRead{ Json::read(PS1::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			const auto& json{ jsonRead.value() };
			m_game.readSettings(json);
			m_bonus.readSettings(json);
			try
			{
				JSON_GET(json, m_showNote);
			}
			catch (const Json::Exception& e)
			{
				Console::append(Console::Type::Exception, Json::exceptionFormat, "Loop", e.what());
			}
		}
	}

	Loop::~Loop()
	{
		Json::Write json;
		m_game.writeSettings(&json);
		m_bonus.writeSettings(&json);
		JSON_SET(&json, m_showNote);
		Json::overwrite(json, PS1::settingsFilePath(Game::name));

		if (isValid())
		{
			m_freecam.enable(false);
			m_freecam.update();
		}
	}

	void Loop::draw()
	{
		if (m_showNote)
		{
			Console::append(Console::Type::Common,
				"({}) Note: During some cutscenes, it is sometimes impossible to receive the camera position, and will be at 0, 0, 0", Game::name);
			m_showNote = false;
		}
		DRAW_GAME_WINDOWS(m_freecam.draw(), m_game.input()->draw(), m_game.settings()->draw(), m_bonus.draw());
		PS1_DEBUG_DRAW_WINDOW;
	}

	void Loop::update()
	{
		CustomCode::update(m_game);
		m_game.update();
		m_freecam.update();
		m_bonus.update();
	}

	bool Loop::isValid()
	{
		const auto op{ Game::offsetPattern(m_game.version()) };
		return m_game.ram().isPatternValid(op.offset, op.pattern);
	}
}
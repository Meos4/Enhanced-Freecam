#include "Loop.hpp"

#include "Common/PS2/Debug.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/Console.hpp"
#include "Common/Json.hpp"

#include "CustomCode.hpp"

namespace PS2::JadeCocoon2
{
	Loop::Loop(Game&& game)
		: m_game(std::move(game)), m_freecam(&m_game)
	{
		const auto jsonRead{ Json::read(PS2::settingsFilePath(Game::name)) };
		if (jsonRead.has_value())
		{
			const auto& json{ jsonRead.value() };
			m_game.readSettings(json);
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
		JSON_SET(&json, m_showNote);
		Json::overwrite(json, PS2::settingsFilePath(Game::name));

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
				"({}) Note: If there are many things displayed on the screen, the game may crash (especially in forests)", Game::name);
			m_showNote = false;
		}
		DRAW_GAME_WINDOWS(m_freecam.draw(), m_game.input()->draw(), m_game.settings()->draw(), ImGui::TextUnformatted("None"));
		PS2_PCSX2_DRAW_PNACH_BEHAVIOR_WINDOW;
		PS2_DEBUG_DRAW_WINDOW;
	}

	void Loop::update()
	{
		CustomCode::update(m_game);
		m_freecam.update();
	}

	bool Loop::isValid()
	{
		const auto [offset, pattern]{ Game::offsetPattern(m_game.version()) };
		return m_game.ram().isPatternValid(offset, pattern);
	}
}
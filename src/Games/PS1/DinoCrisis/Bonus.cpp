#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS1::DinoCrisis
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No Cutscene Black Bar");

		Ui::checkbox(Ui::lol("No Cutscene Black Bar"), &m_noCutsceneBlackBar);
	}

	void Bonus::update()
	{
		m_game->ram().write( m_game->offset().Fn_drawEffects + 0x228, m_noCutsceneBlackBar ? 0x1000006E : 0x1440006E);
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noCutsceneBlackBar);
			}
		}
		catch (const Json::Exception& e)
		{
			Console::append(Console::Type::Exception, Json::exceptionFormat, _Bonus, e.what());
		}
	}

	void Bonus::writeSettings(Json::Write* json)
	{
		auto* const j{ &(*json)[_Bonus] };
		JSON_SET(j, m_noCutsceneBlackBar);
	}
}
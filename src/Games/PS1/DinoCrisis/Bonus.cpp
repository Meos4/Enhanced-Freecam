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
		Ui::checkbox(Ui::lol("No Game Over Fade"), &m_noGameOverFade);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.write(offset.Fn_drawEffects + 0x228, m_noCutsceneBlackBar ? 0x1000006E : 0x1440006E);
		const auto drawGameOverFadeJump{ 0x80000000 + offset.Fn_drawFade + (m_noGameOverFade ? 0x34 : 0x4C) };
		ram.write(offset.drawFadeTbl + 0xC, drawGameOverFadeJump);
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noCutsceneBlackBar);
				JSON_GET(j, m_noGameOverFade);
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
		JSON_SET(j, m_noGameOverFade);
	}
}
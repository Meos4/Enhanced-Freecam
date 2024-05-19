#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::ResidentEvil4
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("Unlock All");

		Ui::separatorText("Cheats");
		Ui::labelXSpacing("Unlock All");
		if (Ui::buttonItemWidth("Set"))
		{
			m_unlockAll = true;
		}
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		if (m_unlockAll)
		{
			const auto current{ ram.read<s32>(offset.progression) };
			ram.write(offset.progression, current | 0xFF'DD'00'0F);
			ram.write(offset.progression + 9, u8(4)); // Ada's report
			Console::append(Console::Type::Success, "All unlocked successfully");
			m_unlockAll = false;
		}
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				
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
		
	}
}
#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

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

		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Game Over"), &m_noGameOver);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("No Collisions"), &m_noCollisions);
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

		ram.write(offset.Fn_drawFog, m_noFog ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFF30, 0x7FB000C0 });
		ram.write(offset.Fn_updateGameOver + 0x70, m_noGameOver ? 0x00009021 : 0x0062900A);
		ram.write(offset.Fn_updatePlayer + 0x4B0, m_noCollisions ? 0x00000000 : Mips::jal(offset.Fn_updatePlayerCollisions));

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
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_noGameOver);
				JSON_GET(j, m_noCollisions);
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
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_noGameOver);
		JSON_SET(j, m_noCollisions);
	}
}
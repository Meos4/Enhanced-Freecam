#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DBZTenkaichi3
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No Near Transparency");

		Ui::checkbox(Ui::lol("No Blur"), &m_noBlur);
		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Near Transparency"), &m_noNearTransparency);
		Ui::slider(Ui::lol("Shaders"), &m_shaders, "%d", ImGuiSliderFlags_AlwaysClamp);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.writeConditional(m_noBlur,
			offset.Fn_drawFarBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF40, 0x24050010 },
			offset.Fn_drawNearBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 }
		);

		ram.writeConditional(m_noAuras, 
			offset.Fn_drawAuras, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 },
			offset.Fn_drawAurasLightning, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFB10008 }
		);

		ram.write(offset.Fn_updateCharNear + 0xB4, m_noNearTransparency ? 0x1000002C : 0x1040002C);
		ram.write(offset.Fn_drawCharTextures + 0x418, Mips::li(Mips::Register::a1, m_shaders));
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noBlur);
				JSON_GET(j, m_noAuras);
				JSON_GET(j, m_noNearTransparency);
				JSON_GET(j, m_shaders);
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
		JSON_SET(j, m_noBlur);
		JSON_SET(j, m_noAuras);
		JSON_SET(j, m_noNearTransparency);
		JSON_SET(j, m_shaders);
	}
}
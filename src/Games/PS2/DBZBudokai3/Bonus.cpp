#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DBZBudokai3
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No On Screen Effects");

		Ui::separatorText("Global");
		if (m_game->version() != Version::NtscU_GH)
		{
			Ui::checkbox(Ui::lol("16:9"), &m_displayMode);
			Ui::hoveredTooltip("Turn the game in 16:9 aspect ratio, this mode is official but unused / unfinished");
		}

		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Cel Shading"), &m_noCelShading);
		Ui::checkbox(Ui::lol("No Shaders Highlight"), &m_noShadersHighlight);
		ImGui::Combo(Ui::lol("Shaders"), &m_shadersMode, "Normal\0No Shaders\0No Textures\0");

		Ui::separatorText("Battle");
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/grey lines");

		Ui::separatorText("Dragon Universe");
		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("Infinite Far Distance"), &m_infFarClip);
		Ui::hoveredTooltip("Remove the far clip limit\n*Showing an important part of the map (Ex: using a high fov) may crash the game");

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
		const auto version{ m_game->version() };

		u32 dmShift,
			dmShift2,
			ddduShift,
			ddmShift,
			dwsShift,
			dwsShift2;

		std::array<Mips_t, 2> daInstr;

		if (version == Version::NtscU_GH)
		{
			dmShift = 0x7C;
			dmShift2 = 0xDC;
			ddduShift = 0xA0;
			ddmShift = 0x9C;
			dwsShift = 0x40;
			dwsShift2 = 0x30;
			daInstr = { 0x27BDFEA0, 0x3C02004D };
		}
		else
		{
			dmShift = 0x84;
			dmShift2 = 0xE8;
			ddduShift = 0xA4;
			ddmShift = 0x88;
			dwsShift = 0x30;
			dwsShift2 = 0x40;
			daInstr = { 0x27BDFEF0, 0x3C02BF80 };

			ram.writeConditional(m_displayMode,
				offset.displayMode, 1, 0,
				offset.displayMode2, 1, 0
			);
		}

		ram.write(offset.Fn_drawAura, m_noAuras ? std::array<Mips_t, 2>{ 0x03E00008, 0x00001021 } : daInstr);
		ram.write(offset.Fn_drawModel + dmShift, m_noCelShading ? 0x00003821 : 0x34470200);
		ram.write(offset.Fn_setTextures + 0xE4, m_noShadersHighlight ? 0x00000000 : 0x14600024);

		auto noShaders = [&](bool enable)
		{
			ram.write(offset.Fn_setTextures + 0x2B0, enable ? 0x00000000 : Mips::jal(offset.Fn_unknown));
		};

		auto noTextures = [&](bool enable)
		{
			ram.write(offset.Fn_drawModel + dmShift2, enable ? 0x00002021 : 0x3C042000);
		};

		switch (m_shadersMode)
		{
		case Bonus::SHADERS_NORMAL:
			noShaders(false);
			noTextures(false);
			break;
		case Bonus::SHADERS_NO_SHADERS:
			noShaders(true);
			noTextures(false);
			break;
		case Bonus::SHADERS_NO_TEXTURES:
			noShaders(false);
			noTextures(true);
			break;
		}

		ram.writeConditional(m_noOnScreenEffects,
			offset.Fn_drawWhiteScreen + dwsShift, 0x00001821, 0x24030003,
			offset.Fn_drawWhiteScreen + dwsShift2, 0x00001821, 0x24030002,
			offset.Fn_drawGreyLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 }
		);

		ram.write(offset.Fn_duDrawDragonUniverse + ddduShift, m_noFog ? 0x44800000 : 0xC6000008);
		ram.write(offset.Fn_duDrawMap + ddmShift, m_infFarClip ? 0x3C02487A : 0x3C02457A);

		if (m_unlockAll)
		{
			constexpr auto allCapsules = []()
			{
				std::array<u8, 579> capsules;
				for (std::size_t i{}; i < capsules.size(); ++i)
				{
					capsules[i] = u8(9);
				}
				return capsules;
			};

			static constexpr u64 allChar{ 0x00'00'0F'FF'78'3F'FF'FF };

			ram.write(offset.progression + 8, allChar);
			ram.write(offset.progression + 0xB8, allChar); // DU
			ram.write(offset.progression + 0x10, u16(0x0FFF)); // Stage
			ram.write(offset.progression + 0xA9, s8(1)); // Dragon Arena
			ram.write(offset.capsules, allCapsules());

			Console::append(Console::Type::Success, "All unlocked successfully, to be effective you must save in settings");
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
				JSON_GET(j, m_displayMode);
				JSON_GET(j, m_noAuras);
				JSON_GET(j, m_noCelShading);
				JSON_GET(j, m_noShadersHighlight);
				JSON_GET_MIN_MAX(j, m_shadersMode, 0, Bonus::SHADERS_COUNT - 1);
				JSON_GET(j, m_noOnScreenEffects);
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_infFarClip);
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
		JSON_SET(j, m_displayMode);
		JSON_SET(j, m_noAuras);
		JSON_SET(j, m_noCelShading);
		JSON_SET(j, m_noShadersHighlight);
		JSON_SET(j, m_shadersMode);
		JSON_SET(j, m_noOnScreenEffects);
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_infFarClip);
	}
}
#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

#include <array>

namespace PS2::DBZInfiniteWorld
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
		Ui::checkbox(Ui::lol("16:9"), &m_displayMode);
		Ui::hoveredTooltip("Turn the game in 16:9 aspect ratio, this mode is official but unused / unfinished");
		Ui::checkbox(Ui::lol("No Auras"), &m_noAuras);
		Ui::checkbox(Ui::lol("No Cel Shading"), &m_noCelShading);
		Ui::checkbox(Ui::lol("No Shaders Highlight"), &m_noShadersHighlight);
		ImGui::Combo(Ui::lol("Shaders"), &m_shadersMode, "Normal\0No Shaders\0No Textures\0");

		Ui::separatorText("Battle");
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/grey lines");

		Ui::separatorText("Cheats");
		MiscModel::drawEnableButton("Unlock All", "Set", &m_unlockAll);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.writeConditional(m_displayMode,
			offset.Fn_initSelectStages + 0x38, 0x24040003, 0x0000202D,
			offset.Fn_drawAspectRatio + 0x5C, 0x24100003, 0x8F908A0C,
			offset.Fn_initMovies + 0x4C, 0x24040003, 0x0000202D,
			offset.Fn_initTitleScreen + 0x60, 0x24040003, 0x0000202D,
			offset.Fn_drawText + 0x204, 0x24040003, 0x0000202D,
			offset.Fn_drawGreenRectangle + 0xC, 0x24040003, 0x0000202D
		);

		ram.write(offset.Fn_drawAura,
			m_noAuras ? std::array<Mips_t, 2>{ 0x03E00008, 0x00001021 } : std::array<Mips_t, 2>{ 0x27BDFED0, 0x3C020055 });

		ram.write(offset.Fn_drawModel + 0xCC, m_noCelShading ? 0x00000000 : Mips::jal(offset.Fn_unknown2));
		ram.write(offset.Fn_setTextures + 0xCC, m_noShadersHighlight ? 0x00000000 : 0x14600024);

		auto noShaders = [&](bool enable)
		{
			ram.write(offset.Fn_setTextures + 0x290, enable ? 0x00000000 : Mips::jal(offset.Fn_unknown));
		};

		auto noTextures = [&](bool enable)
		{
			ram.write(offset.Fn_drawTexturesOnShaders, enable ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFD0, 0xFFBF0020 });
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
			offset.Fn_drawWhiteScreen + 0x30, 0x00001821, 0x24030003,
			offset.Fn_drawWhiteScreen + 0x2C, 0x00001821, 0x24030002,
			offset.Fn_drawGreyLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFB0, 0xFFBF0040 }
		);

		if (m_unlockAll)
		{
			constexpr auto completion = []<u32 Size, u8 Data>()
			{
				std::array<u8, Size> arr;
				for (u32 i{}; i < Size; ++i)
				{
					arr[i] = Data;
				}
				return arr;
			};

			ram.write(offset.progression, completion.operator()<1103, 0xE0>()); // Capsules | Char | Stages | PadLock | Fighter's Road
			ram.write(offset.progression + 0x834, s8(1)); // Dragon Mission Continue
			ram.write(offset.progression + 0x44F, completion.operator()<269, 0xE1>()); // Dragon Mission | Fighter's Road

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
				JSON_GET_MIN_MAX(j, m_shadersMode, 0, Bonus::SHADERS_COUNT - 1);
				JSON_GET(j, m_noShadersHighlight);
				JSON_GET(j, m_noOnScreenEffects);
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
		JSON_SET(j, m_shadersMode);
		JSON_SET(j, m_noShadersHighlight);
		JSON_SET(j, m_noOnScreenEffects);
	}
}
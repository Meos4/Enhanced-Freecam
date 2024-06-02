#include "Bonus.hpp"

#include "Common/Buffer.hpp"
#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
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
		Ui::checkbox(Ui::lol("No On Screen Effects"), &m_noOnScreenEffects);
		Ui::hoveredTooltip("Hide on screen effects\nEx: white screen/lines");
		Ui::slider(Ui::lol("Shaders"), &m_shaders, "%d", ImGuiSliderFlags_AlwaysClamp);

		Ui::separatorText("Cheats");
		MiscModel::drawEnableButton("Unlock All", "Set", &m_unlockAll);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto version{ m_game->version() };

		ram.writeConditional(m_noBlur,
			offset.Fn_drawFarBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFF40, 0x24050010 },
			offset.Fn_drawNearBlur, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 }
		);

		ram.writeConditional(m_noAuras, 
			offset.Fn_drawAuras, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 },
			offset.Fn_drawAurasLightning, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB00000 }
		);

		ram.write(offset.Fn_updateCharNear + 0xB4, m_noNearTransparency ? 0x1000002C : 0x1040002C);

		const auto dwlInstr{ version == Version::Pal ? 0x8F82AB80 : 0x8F82A800 };

		ram.writeConditional(m_noOnScreenEffects,
			offset.Fn_drawWhiteScreen, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFD0, 0x3C02002F },
			offset.Fn_drawShade, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFFE0, 0xFFB20010 },
			offset.Fn_drawWhiteLines, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x27BDFDB0, dwlInstr },
			offset.Fn_drawBlurCutscene, Mips::jrRaNop(), std::array<Mips_t, 2>{ 0x00A0382D, 0x3C05FF00 }
		);

		ram.write(offset.Fn_drawCharTextures + 0x418, Mips::li(Mips::Register::a1, m_shaders));

		if (m_unlockAll)
		{
			const auto progressionPtr{ ram.read<u32>(offset.progressionPtr) };

			if (progressionPtr)
			{
				static constexpr std::array<u8, 132> progression
				{
					0x3D, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00,
					0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00,
					0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
					0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
					0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00,
					0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x3D, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
					0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x01, 0x00, 0x00, 0x00
				};

				static constexpr std::array<u8, 32> progression2
				{
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Char 1
					0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Char 2
					0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, // Char 3
					0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00  // Stage
				};

				static constexpr std::array<u8, 152> items
				{
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
					0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
				};

				struct ExpLevel
				{
					s32 exp;
					u16 level;
				};

				static constexpr std::array<ExpLevel, 97> expLevel
				{
					20000, 4, 15000, 5, 20000, 4, 20000, 4,
					16000, 4, 20000, 4, 20000, 4, 20000, 4,
					20000, 4, 21000, 3, 20000, 4, 19000, 4,
					15000, 5, 19000, 4, 21000, 3, 20000, 4,
					21000, 3, 21000, 3, 25000, 3, 15000, 5,
					15000, 5, 20000, 4, 20000, 4, 18000, 4,
					15000, 5, 20000, 4, 20000, 4, 15000, 5,
					15000, 5, 15000, 5, 20000, 4, 20000, 4,
					18000, 4, 19000, 4, 15000, 5, 20000, 4,
					19000, 4, 20000, 4, 16000, 4, 15000, 5,
					16000, 4, 19000, 4, 25000, 3, 15000, 3,
					18000, 4, 18000, 4, 15000, 5, 25000, 3,
					19000, 4, 25000, 5, 20000, 4, 24000, 4,
					24000, 4, 15000, 3, 15000, 3, 15000, 3,
					19000, 4, 20000, 4, 21000, 3, 25000, 2,
					15000, 5, 19000, 4, 19000, 4, 20000, 4,
					18000, 4, 25000, 3, 24000, 4, 19000, 4,
					20000, 4, 16000, 4, 25000, 3, 15000, 3,
					15000, 3, 25000, 2, 18000, 4, 25000, 3,
					25000, 3, 20000, 4, 20000, 4, 25000, 3,
					24000, 4, 25000, 2, 25000, 2, 25000, 2,
					25000, 3, 18000, 4, 18000, 4, 20000, 4,
					18000, 4, 18000, 4, 24000, 4, 15000, 5,
					15000, 5, 15000, 5, 20000, 4, 24000, 4,
					21000, 3
				};

				static constexpr auto evoZSize{ 56 * 97 };
				Buffer charData(evoZSize);
				auto* const charDataPtr{ charData.data() };
				ram.read(progressionPtr + 0x1808, charDataPtr, evoZSize);

				for (s32 i{}; i < 97; ++i)
				{
					auto* const expPtr{ charDataPtr + i * 56 + 0x30 };
					*(s32*)expPtr = expLevel[i].exp;
					*(u16*)(expPtr + sizeof(s32)) = expLevel[i].level;
				}

				ram.write(progressionPtr + 8, 0x000001FF); // Dragon Ball | Skip Dragon History Goku Dialog
				ram.write(progressionPtr + 0x10, progression); // Dragon History
				ram.write(progressionPtr + 0x208, 0x0000001400000001); // Ultimate Battle Mission 100 | Survival
				ram.write(progressionPtr + 0xC10, progression2);
				ram.write(progressionPtr + 0x1808, *charDataPtr, evoZSize);
				ram.write(progressionPtr + 0x2EC8, items);

				if (version == Version::NtscJ)
				{
					ram.write(progressionPtr + 0xC30, 0x01FFFFFF); // Ost
				}

				Console::append(Console::Type::Success, "All unlocked successfully");
			}
			else
			{
				Console::append(Console::Type::Error, "Can't unlock all");
			}

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
				JSON_GET(j, m_noBlur);
				JSON_GET(j, m_noAuras);
				JSON_GET(j, m_noNearTransparency);
				JSON_GET(j, m_noOnScreenEffects);
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
		JSON_SET(j, m_noOnScreenEffects);
		JSON_SET(j, m_shaders);
	}
}
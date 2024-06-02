#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::DragonQuest8
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No Near Clip Transparency");

		Ui::checkbox(Ui::lol("No Near Clip Transparency"), &m_noNearClipTransparency);
		Ui::hoveredTooltip("Prevents entities from disappearing on the screen when the camera is too close");

		const auto timeInt{ static_cast<u32>(m_timer.time) };
		struct
		{
			u32 hour;
			u32 minute;
		} timer{ timeInt, static_cast<u32>((m_timer.time - timeInt) * 60) };

		Ui::labelXSpacing("Time");
		ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
		m_timer.shouldChangeValue |= Ui::dragWrap("##Hour", &timer.hour, 1u, "%dh", 0, 0u, 23u);
		ImGui::SameLine();
		m_timer.shouldChangeValue |= Ui::dragWrap("##Time", &timer.minute, 1u, "%dm", 0, 0u, 59u);
		ImGui::PopItemWidth();
		if (m_timer.shouldChangeValue)
		{
			m_timer.time = static_cast<float>(timer.hour) + static_cast<float>(timer.minute / 59.99f);
		}
		ImGui::SameLine();
		Ui::checkbox("Frozen", &m_timer.isFrozen);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("No Random Encounter"), &m_noRandomEncounter);
		MiscModel::drawEnableButton("Bag items", "Unlock all", &m_shouldGiftItems);
		Ui::hoveredTooltip("Gift all items in the bag in 999 copies");
		MiscModel::drawEnableButton("Stats", "Set max", &m_shouldStatsMax);
		Ui::hoveredTooltip("Set stats at max for Hero, Yangus, Jessica and Angelo");
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		const Mips_t tsInstr{ m_game->version() == Version::Pal ? 0xE4816B10 : 0xE4816B00 };

		ram.write(offset.Fn_randomEncounterBehavior + 0x18C, m_noRandomEncounter ? 0x00000000 : 0xE6410024);
		ram.write(offset.Fn_TimeStep__8dqCSceneFf + 0xBC, m_timer.isFrozen ? 0x00000000 : tsInstr);
		ram.writeConditional(m_noNearClipTransparency,
			offset.Fn_DrawStep__12CModelObjectFf + 0x5C, 0x10000006, 0x54600006,
			offset.Fn_drawMainCharacter + 0x3BC, 0x00000000, 0x10400003
		);

		if (m_timer.shouldChangeValue || m_timer.isFrozen)
		{
			ram.write(offset.timer, m_timer.time);
			m_timer.shouldChangeValue = false;
		}
		else if (!m_timer.isFrozen)
		{
			ram.read(offset.timer, &m_timer.time);
		}

		if (m_shouldGiftItems)
		{
			constexpr auto getItems999 = []()
			{
				std::array<u32, 388> items;
				for (std::size_t i{}; i < items.size(); ++i)
				{
					items[i] = (static_cast<u32>(i) + 1) + (999 << 16);
				}
				return items;
			};

			static constexpr auto items999{ getItems999() };
			ram.write(offset.bag, items999);
			Console::append(Console::Type::Success, "Generated items successfully");
			m_shouldGiftItems = false;
		}

		if (m_shouldStatsMax)
		{
			auto currentOffset{ offset.heroStats };

			// Hero, Yangus, Jessica, Angelo
			for (s32 i{}; i < 4; ++i)
			{
				// Max hp, Hp, Max mp, Mp
				ram.write(currentOffset, std::array<s32, 4>{ 999, 999, 999, 999 });

				// Level
				currentOffset += 0x10;
				ram.write(currentOffset, s16(98));

				// Spells
				currentOffset += 0x18;
				ram.write(currentOffset, -1);

				// Str, agi, res, wis
				currentOffset += 8;
				ram.write(currentOffset, std::array<s16, 4>{ 999, 999, 999, 999 });

				// Skills
				currentOffset += 0xA;
				ram.write(currentOffset, std::array<s16, 5>{ 100, 100, 100, 100, 100 });

				// Next character
				currentOffset += 0x2E;
			}

			Console::append(Console::Type::Success, "Set stats max successfully");
			m_shouldStatsMax = false;
		}
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noNearClipTransparency);
				JSON_GET(j, m_timer.isFrozen);
				if (m_timer.isFrozen)
				{
					JSON_GET_MIN_MAX(j, m_timer.time, 0.f, 24.f);
				}
				JSON_GET(j, m_noRandomEncounter);
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
		JSON_SET(j, m_noNearClipTransparency);
		JSON_SET(j, m_timer.time);
		JSON_SET(j, m_timer.isFrozen);
		JSON_SET(j, m_noRandomEncounter);
	}
}
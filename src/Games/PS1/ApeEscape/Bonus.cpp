#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS1::ApeEscape
{
	enum
	{
		TITLE_SCREEN_NONE,
		TITLE_SCREEN_OPTION_MENU = 4,
		TITLE_SCREEN_MINI_GAME = 5,
		TITLE_SCREEN_SOUND_TEST = 6
	};

	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		static constexpr std::array<Ui::LabelSetter<s32>, 3> titleScreenChoices
		{
			"Option", TITLE_SCREEN_OPTION_MENU,
			"Mini Game", TITLE_SCREEN_MINI_GAME,
			"Sound Test", TITLE_SCREEN_SOUND_TEST
		};

		Ui::setXSpacingStr("Debug Menu");

		Ui::separatorText("Global");
		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Tv Effect"), &m_noTvEffect);

		Ui::separatorText("Title Screen");
		Ui::checkbox(Ui::lol("No Timer"), &m_titleScreenNoTimer);
		Ui::labelXSpacing("Debug Menu");
		Ui::buttonsSetter<s32>(titleScreenChoices, &m_titleScreenState);

		Ui::separatorText("Stage Select");
		MiscModel::drawEnableButton("Debug Menu", "Select Menu", &m_stageSelectSelectMenu);

		Ui::separatorText("Stage Preview");
		Ui::checkbox(Ui::lol("No Timer##2"), &m_stagePreviewNoTimer);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto state{ m_game->state() };
		const auto version{ m_game->version() };

		u32 cShift,
			tpShift,
			tsShift,
			ssShift;

		Mips_t tvInstr;

		if (version == Version::NtscU)
		{
			cShift = 0x43C4;
			tpShift = 0x2490;
			tsShift = 0x1810;
			ssShift = 0x3BDC;
			tvInstr = 0x3C031B4E;
		}
		else
		{
			cShift = 0x44CC;
			tpShift = 0x2504;
			tsShift = 0x1820;
			ssShift = 0x3B10;
			tvInstr = 0x3C031B4E;
		}

		if (version == Version::NtscJRev1)
		{
			tvInstr = 0x27A50010;
		}

		ram.writeConditional(m_noFog,
			offset.Fn_drawActive + 0x90, 0x24047FFF, 0x94440028,
			offset.Fn_drawActive + 0xEC, 0x24057FFF, 0x94450028,
			offset.Fn_drawLayer + 0x68, 0x24097FFF, 0x8E49002C,
			offset.Fn_drawLayer + 0xB4, 0x24087FFF, 0x96480028,
			offset.Fn_drawLayer2 + 0x124, 0x24057FFF, 0x94E50028,
			offset.Fn_drawLayer2 + 0x12C, 0x24067FFF, 0x94460016,
			offset.Fn_drawEnemy + 0x12C, 0x24047FFF, 0x94440028
		);

		if (state == State::TitleScreen)
		{
			ram.write(offset.overlay + 0x1DC, m_titleScreenNoTimer ? 0x10000005 : 0x14400005);
		}
		else if (state == State::Cutscene)
		{
			ram.write(offset.overlay + cShift, m_noTvEffect ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFB8, tvInstr });
		}
		else if (state == State::TrainingPreview)
		{
			ram.write(offset.overlay + tpShift, m_noTvEffect ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFFB8, tvInstr });
		}
		else if (state == State::StagePreview)
		{
			ram.write(offset.overlay + 0x1F20, m_stagePreviewNoTimer ? 0x1000005D : 0x1462005D);
		}

		if (m_titleScreenState != TITLE_SCREEN_NONE)
		{
			if (state == State::TitleScreen)
			{
				ram.write(offset.overlay + tsShift, m_titleScreenState);
			}
			else
			{
				Console::append(Console::Type::Error, "Not in title screen");
			}
			m_titleScreenState = TITLE_SCREEN_NONE;
		}

		if (m_stageSelectSelectMenu)
		{
			if (state == State::StageSelect)
			{
				ram.write<s32>(offset.overlay + ssShift, 0);
			}
			else
			{
				Console::append(Console::Type::Error, "Not in stage select");
			}
			m_stageSelectSelectMenu = false;
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
				JSON_GET(j, m_noTvEffect);
				JSON_GET(j, m_titleScreenNoTimer);
				JSON_GET(j, m_stagePreviewNoTimer);
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
		JSON_SET(j, m_noTvEffect);
		JSON_SET(j, m_titleScreenNoTimer);
		JSON_SET(j, m_stagePreviewNoTimer);
	}
}
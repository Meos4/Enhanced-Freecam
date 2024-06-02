#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::HauntingGround
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No Cutscene Black Bar");

		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Shade"), &m_noShade);
		Ui::checkbox(Ui::lol("No Blur"), &m_noBlur);
		Ui::checkbox(Ui::lol("No Textures"), &m_noTextures);
		Ui::checkbox(Ui::lol("No Subtitles"), &m_noSubtitles);
		Ui::checkbox(Ui::lol("No Cutscene Effect"), &m_noCutsceneEffect);
		Ui::hoveredTooltip("Remove on screen cutscene effect");
		Ui::checkbox(Ui::lol("No Cutscene Black Bar"), &m_noCutsceneBlackBar);
		Ui::checkbox(Ui::lol("Run By Default"), &m_runByDefault);
		Ui::hoveredTooltip("Fiona run by default and can walk by holding the running button");

		Ui::separatorText("Cheats");
		MiscModel::drawEnableButton("Secret Room", "Unlock All", &m_shouldUnlockAllSecretRoom);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.write(offset.Fn_drawFog + 0x7C, m_noFog ? 0x100001D6 : 0x104001D6);
		ram.write(offset.Fn_drawShade + 0x48, m_noShade ? 0x100004B2 : 0x104004B2);
		ram.write(offset.Fn_drawBlur + 0x4C, m_noBlur ? 0x10000804 : 0x12200804);
		ram.writeConditional(m_noTextures,
			offset.Fn_drawEntity + 0xA4, 0x10000031, 0x12420031,
			offset.Fn_drawEntity + 0x1C8, 0x1000000A, 0x1040000A, // Cutscene
			offset.Fn_drawStaticMap + 0x19C, 0x10000007, 0x10820007,
			offset.Fn_drawDynamicMap + 0x148, 0x10000007, 0x10820007
		);
		ram.write(offset.Fn_drawSubtitles + 0xC, m_noSubtitles ? 0x10000006 : 0x10A00006);
		ram.write(offset.Fn_cutsceneFrame + 0xA4, m_noCutsceneEffect ? 0x1000001B : 0x1060001B);
		ram.write(offset.Fn_blackBarCutscene + 0xC, m_noCutsceneBlackBar ? 0x00000000 : 0x10600003);
		ram.write(offset.Fn_fionaMovementControls + 0x554, m_runByDefault ? 0x146001AA : 0x106001AA);

		if (m_shouldUnlockAllSecretRoom)
		{
			Mips::Register utsReg;

			u32 umugShift,
				umogShift,
				uagShift;

			if (m_game->version() == Version::Pal)
			{
				utsReg = Mips::Register::a2;
				umugShift = 0x258;
				umogShift = 0x5CC;
				uagShift = 0x240;
			}
			else
			{
				utsReg = Mips::Register::a3;
				umugShift = 0x230;
				umogShift = 0x5AC;
				uagShift = 0x218;
			}

			ram.write(offset.Fn_updateTitleScreen + 0x64, Mips::li(utsReg, 1)); // Cursor
			ram.write(offset.Fn_drawTitleScreen + 0x190, 0x24020001); // Text
			ram.write(offset.Fn_drawTitleScreen + 0x1CC, 0x24020001); // Cursor Light
			ram.write(offset.Fn_updateMusicGallery + umugShift, 0x00000000); // Title
			ram.write(offset.Fn_musicGalleryPlayMusic + 0x88, 0x1000008C);
			ram.write(offset.Fn_updateMovieGallery + 0x10C, 0x100000CA); // Play Movie
			ram.write(offset.Fn_updateMovieGallery + umogShift, 0x00000000); // Title
			ram.write(offset.Fn_updateMovieGallery + umogShift + 0x288, 0x1000006C); // Preview
			ram.write(offset.Fn_updateArtGallery + uagShift, 0x00000000); // Title
			ram.write(offset.Fn_artGalleryPlayImage + 0xC4, 0x100000B8);
			ram.write(offset.Fn_isCharacterGalleryUnlocked + 0x120, 0x24020001);
			ram.write(offset.Fn_initCostumeMenu + 0xF0, 0x00000000); // Texas Cowgirl
			ram.write(offset.Fn_initCostumeMenu + 0x104, 0x00000000); // Frog
			ram.write(offset.Fn_initCostumeMenu + 0x1EC, 0x00000000); // Stuffed Toy
			ram.write(offset.Fn_initCostumeMenu + 0x1FC, 0x10000004); // Stuffed Toy

			Console::append(Console::Type::Success, "All unlocked successfully");
			m_shouldUnlockAllSecretRoom = false;
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
				JSON_GET(j, m_noShade);
				JSON_GET(j, m_noBlur);
				JSON_GET(j, m_noTextures);
				JSON_GET(j, m_noSubtitles);
				JSON_GET(j, m_noCutsceneEffect);
				JSON_GET(j, m_noCutsceneBlackBar);
				JSON_GET(j, m_runByDefault);
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
		JSON_SET(j, m_noShade);
		JSON_SET(j, m_noBlur);
		JSON_SET(j, m_noTextures);
		JSON_SET(j, m_noSubtitles);
		JSON_SET(j, m_noCutsceneEffect);
		JSON_SET(j, m_noCutsceneBlackBar);
		JSON_SET(j, m_runByDefault);
	}
}
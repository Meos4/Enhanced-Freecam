#pragma once

#include "Common/Json.hpp"

namespace PS2::HauntingGround
{
	class Game;

	class Bonus final
	{
	public:
		Bonus(Game* game);

		void draw();
		void update();
		void readSettings(const Json::Read& json);
		void writeSettings(Json::Write* json);
	private:
		Game* m_game;
		bool m_noFog{};
		bool m_noShade{};
		bool m_noBlur{};
		bool m_noTextures{};
		bool m_noSubtitles{};
		bool m_noCutsceneEffect{};
		bool m_noCutsceneBlackBar{};
		bool m_runByDefault{};
		bool m_shouldUnlockAllSecretRoom{};
	};
}
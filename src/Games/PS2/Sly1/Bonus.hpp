#pragma once

#include "Common/Json.hpp"

namespace PS2::Sly1
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
		bool m_noMotionBlur{};
		bool m_invulnerable{};
		bool m_shouldUnlockAllKeys{};
		bool m_shouldUnlockAllPowerUps{};
	};
}
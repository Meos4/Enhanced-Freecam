#pragma once

#include "Common/Json.hpp"

namespace PS2::ResidentEvil4
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
		bool m_noGameOver{};
		bool m_noCollisions{};
		bool m_unlockAll{};
	};
}
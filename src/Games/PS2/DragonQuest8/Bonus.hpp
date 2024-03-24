#pragma once

#include "Common/Json.hpp"

namespace PS2::DragonQuest8
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
		struct
		{
			float time;
			bool shouldChangeValue;
			bool isFrozen;
		} m_timer{};
		bool m_noNearClipTransparency{};
		bool m_noRandomEncounter{};
		bool m_shouldGiftItems{};
		bool m_shouldStatsMax{};
	};
}
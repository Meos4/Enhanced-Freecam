#pragma once

#include "Common/Json.hpp"

namespace PS2::DBZTenkaichi3
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
		bool m_noBlur{};
		bool m_noAuras{};
		bool m_noNearTransparency{};
		bool m_noOnScreenEffects{};
		u8 m_shaders{ 128 };
		bool m_unlockAll{};
	};
}
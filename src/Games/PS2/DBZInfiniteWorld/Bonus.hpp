#pragma once

#include "Common/Json.hpp"

namespace PS2::DBZInfiniteWorld
{
	class Game;

	class Bonus final
	{
	public:
		enum
		{
			SHADERS_NORMAL,
			SHADERS_NO_SHADERS,
			SHADERS_NO_TEXTURES,
			SHADERS_COUNT
		};

		Bonus(Game* game);

		void draw();
		void update();
		void readSettings(const Json::Read& json);
		void writeSettings(Json::Write* json);
	private:
		Game* m_game;
		bool m_displayMode{};
		bool m_noAuras{};
		bool m_noCelShading{};
		bool m_noShadersHighlight{};
		s32 m_shadersMode{ Bonus::SHADERS_NORMAL };
		bool m_noOnScreenEffects{};
		bool m_unlockAll{};
	};
}
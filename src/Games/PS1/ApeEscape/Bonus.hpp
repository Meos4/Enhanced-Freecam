#pragma once

#include "Common/Json.hpp"

namespace PS1::ApeEscape
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
		s32 m_titleScreenState{};
		bool m_noFog{};
		bool m_noTvEffect{};
		bool m_titleScreenNoTimer{};
		bool m_stageSelectSelectMenu{};
		bool m_stagePreviewNoTimer{};
	};
}
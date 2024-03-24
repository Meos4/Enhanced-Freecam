#pragma once

namespace PS2::DragonQuest8
{
	class Game;

	class Misc final
	{
	public:
		Misc(Game* game);

		void draw();
		void update();
		void enable(bool enable);
	private:
		Game* m_game;
		bool m_isGamePaused{};
		bool m_isHudHidden{};
		bool m_isEnabled{};
	};
}
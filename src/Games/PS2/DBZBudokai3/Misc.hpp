#pragma once

namespace PS2::DBZBudokai3
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
		float m_timescale{};
		bool m_isEnabled{};
	};
}
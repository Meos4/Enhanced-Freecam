#pragma once

namespace PS2::HauntingGround
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
		bool m_isEnabled{};
		bool m_isGamePaused{};
	};
}
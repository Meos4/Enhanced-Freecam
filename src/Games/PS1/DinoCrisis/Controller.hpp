#pragma once

namespace PS1::DinoCrisis
{
	class Game;

	class Controller final
	{
	public:
		Controller(Game* game);

		void draw();
		void update();
		void enable(bool enable);
	private:
		Game* m_game;
		bool m_isButtonEnabled{ true };
		bool m_isEnabled{};
	};
}
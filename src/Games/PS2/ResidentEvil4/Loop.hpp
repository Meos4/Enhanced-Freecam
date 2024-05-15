#pragma once

#include "Common/GameInfo.hpp"

#include "Bonus.hpp"
#include "Freecam.hpp"
#include "Game.hpp"

namespace PS2::ResidentEvil4
{
	class Loop final : public GameLoop
	{
	public:
		Loop(Game game);
		~Loop();

		void draw() override;
		void update() override;
		bool isValid() override;
	private:
		Game m_game;
		Freecam m_freecam;
		Bonus m_bonus;
	};
}
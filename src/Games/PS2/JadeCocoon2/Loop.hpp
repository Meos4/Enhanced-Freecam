#pragma once

#include "Common/GameInfo.hpp"

#include "Freecam.hpp"
#include "Game.hpp"

namespace PS2::JadeCocoon2
{
	class Loop final : public GameLoop
	{
	public:
		Loop(Game&& game);
		~Loop();

		void draw() override;
		void update() override;
		bool isValid() override;
	private:
		Game m_game;
		Freecam m_freecam;
		bool m_showNote{ true };
	};
}
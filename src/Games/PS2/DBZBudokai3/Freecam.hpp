#pragma once

#include "Common/Camera3DControls.hpp"

#include "Camera.hpp"
#include "Controller.hpp"
#include "Misc.hpp"

namespace PS2::DBZBudokai3
{
	class Game;

	class Freecam final
	{
	public:
		Freecam(Game* game);

		void draw();
		void update();
		void enable(bool enable);
	private:
		Game* m_game;
		Camera m_camera;
		Misc m_misc;
		Controller m_controller;
		Camera3DControls m_controls;
		bool m_isEnabled{};
	};
}
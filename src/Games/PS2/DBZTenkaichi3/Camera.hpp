#pragma once

#include "Common/Vec3.hpp"

#include <utility>

namespace PS2::DBZTenkaichi3
{
	class Game;

	class Camera final
	{
	public:
		Camera(Game* game);

		std::pair<u32, u32> vmPosPtr(s32 state) const;
		void draw();
		void update();
		void enable(bool enable);
		void moveForward(float amount);
		void moveRight(float amount);
		void moveUp(float amount);
		void rotateX(float amount);
		void rotateY(float amount);
		void rotateZ(float amount);
		void read();
		void write();
		void enableGameCamera(bool enable);
	private:
		Game* m_game;
		Vec3<float> m_position{};
		Vec3<float> m_rotation{};
		bool m_isEnabled{};
	};
}
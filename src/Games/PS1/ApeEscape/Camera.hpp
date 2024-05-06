#pragma once

#include "Common/Types.hpp"
#include "Common/Vec3.hpp"

namespace PS1::ApeEscape
{
	class Game;

	class Camera final
	{
	public:
		Camera(Game* game);

		u32 vmPtr(s32 state, bool read) const;
		void writeProjectionMatrix(s16 x, s16 y);
		void draw();
		void update();
		void enable(bool enable);
		void moveForward(float amount);
		void moveRight(float amount);
		void moveUp(float amount);
		void rotateX(float amount);
		void rotateY(float amount);
		void rotateZ(float amount);
		void increaseFov(float amount);
		void read();
		void write();
		void enableGameCamera(bool enable);
	private:
		Game* m_game;
		Vec3<s16> m_position{};
		Vec3<float> m_positionFp{};
		Vec3<float> m_rotation{};
		float m_fov{};
		bool m_isEnabled{};
	};
}
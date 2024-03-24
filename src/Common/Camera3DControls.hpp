#pragma once

#include "Types.hpp"

class InputWrapper;

struct Input3D
{
	s32 forward;
	s32 backward;
	s32 right;
	s32 left;
	s32 up;
	s32 down;
	s32 rotateRollPos;
	s32 rotateRollNeg;
	s32 rotatePitchPos;
	s32 rotatePitchNeg;
	s32 rotateYawPos;
	s32 rotateYawNeg;
	s32 fovPos;
	s32 fovNeg;
};

class Camera3DControls final
{
public:
	Camera3DControls(InputWrapper* input, const Input3D& input3D);

	float forwardVelocity(float dt);
	float rightVelocity(float dt);
	float upVelocity(float dt);
	float rollVelocity(float dt);
	float pitchVelocity(float dt);
	float yawVelocity(float dt);
	float fovVelocity(float dt);
	void resetVelocity();
private:
	InputWrapper* m_input;
	const Input3D& m_input3D;
	float m_forwardVel{};
	float m_rightVel{};
	float m_upVel{};
};
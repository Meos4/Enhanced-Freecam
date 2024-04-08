#pragma once

#include "Types.hpp"

class InputWrapper;

class Camera3DControls final
{
public:
	Camera3DControls(InputWrapper* input);

	float forwardVelocity(s32 pos, s32 neg);
	float rightVelocity(s32 pos, s32 neg);
	float upVelocity(s32 pos, s32 neg);
	float rollVelocity(s32 pos, s32 neg);
	float pitchVelocity(s32 pos, s32 neg);
	float yawVelocity(s32 pos, s32 neg);
	float fovVelocity(s32 pos, s32 neg);
	void resetVelocity();
private:
	InputWrapper* m_input;
	float m_forwardVel{};
	float m_rightVel{};
	float m_upVel{};
};
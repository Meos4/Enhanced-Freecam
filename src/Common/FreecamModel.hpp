#pragma once

#include "Types.hpp"

#include <span>

class InputWrapper;

namespace FreecamModel
{
	inline constexpr auto cameraNameMonoState{ "Common" };

	void drawCameraName(const char* name);
	void drawCameraName(std::span<const char* const> names, s32 current);
	bool drawEnable(bool* isEnabled);
	void drawSeparatorProperties();
	void drawSeparatorOthers();
	void draw3DSettings();
	void draw3DSettingsNoFov();
	void increaseMovementSpeed(InputWrapper* input, s32 id);
	void decreaseMovementSpeed(InputWrapper* input, s32 id);
	void increaseRotationSpeed(InputWrapper* input, s32 id);
	void decreaseRotationSpeed(InputWrapper* input, s32 id);
	void increaseFovSpeed(InputWrapper* input, s32 id);
	void decreaseFovSpeed(InputWrapper* input, s32 id);
	void update3DSettings(InputWrapper* input, s32 im, s32 dm, s32 ir, s32 dr, s32 ifo, s32 df);
	void update3DSettingsNoFov(InputWrapper* input, s32 im, s32 dm, s32 ir, s32 dr);
	float velocityInterpolation(float vel, float target, float dt);
	float movementVelocity(InputWrapper* input, s32 pos, s32 neg, float* velocity, float dt);
	float rotationVelocity(InputWrapper* input, s32 pos, s32 neg, float dt);
	float fovVelocity(InputWrapper* input, s32 pos, s32 neg, float dt);
}
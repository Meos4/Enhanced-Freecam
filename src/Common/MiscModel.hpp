#pragma once

#include "Types.hpp"
#include "Ui.hpp"

#include <span>

class InputWrapper;

namespace MiscModel
{
	void drawFlags(const char* label, std::span<const Ui::LabelFlag> lf, bool readOnly);
	void drawMiscPauseGame(bool* pauseGame, bool readOnly);
	void drawMiscHideHud(bool* hideHud, bool readOnly);
	void drawMiscPauseGameHideHud(bool* pauseGame, bool* hideHud, bool readOnly);
	void drawControllerButton(bool* button, bool readOnly);
	void drawControllerButtonJoystick(bool* button, bool* joystick, bool readOnly);
	void drawControllerButtonLRJoystick(bool* button, bool* lJoystick, bool* rJoystick, bool readOnly);
	void drawTimescale(float* timescale, float min, float max, bool readOnly);
	void increaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max);
	void decreaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max);
}
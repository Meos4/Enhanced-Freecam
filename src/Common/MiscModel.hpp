#pragma once

#include "Types.hpp"
#include "Ui.hpp"

#include <span>

class InputWrapper;

namespace MiscModel
{
	void drawFlags(const char* label, std::span<const Ui::LabelFlag> lf, bool readOnly);
	void drawTimescale(float* timescale, float min, float max, bool readOnly);
	void increaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max);
	void decreaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max);
}
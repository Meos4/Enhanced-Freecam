#include "MiscModel.hpp"

#include "InputWrapper.hpp"
#include "Ui.hpp"

namespace MiscModel
{
	void drawFlags(const char* label, std::span<const Ui::LabelFlag> lf, bool readOnly)
	{
		Ui::labelXSpacing(label);
		Ui::buttonsFlags(lf, readOnly);
	}

	void drawTimescale(float* timescale, float min, float max, bool readOnly)
	{
		const auto flags{ (readOnly ? ImGuiSliderFlags_ReadOnly : ImGuiSliderFlags_None) | ImGuiSliderFlags_AlwaysClamp };
		Ui::slider(Ui::lol("Timescale"), timescale, "%.2f", flags, min, max);
	}

	void increaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max)
	{
		if (input->isPressed(id))
		{
			*timescale = std::clamp(*timescale * 1.25f, min, max);
		}
	}

	void decreaseTimescale(InputWrapper* input, s32 id, float* timescale, float min, float max)
	{
		if (input->isPressed(id))
		{
			*timescale = std::clamp(*timescale / 1.25f, min, max);
		}
	}
}
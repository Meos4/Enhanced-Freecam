#include "MiscModel.hpp"

#include "InputWrapper.hpp"
#include "Ui.hpp"

#include <array>

namespace MiscModel
{
	void drawFlags(const char* label, std::span<const Ui::LabelFlag> lf, bool readOnly)
	{
		Ui::labelXSpacing(label);
		Ui::buttonsFlags(lf, readOnly);
	}

	void drawControllerButton(bool* button, bool readOnly)
	{
		const Ui::LabelFlag lf{ "Button", button };
		MiscModel::drawFlags("Controller", { &lf, 1 }, readOnly);
	}

	void drawControllerButtonJoystick(bool* button, bool* joystick, bool readOnly)
	{
		const std::array<Ui::LabelFlag, 2> lf
		{
			"Button", button,
			"Joystick", joystick
		};

		MiscModel::drawFlags("Controller", lf, readOnly);
	}

	void drawControllerButtonLRJoystick(bool* button, bool* lJoystick, bool* rJoystick, bool readOnly)
	{
		const std::array<Ui::LabelFlag, 3> lf
		{
			"Button", button,
			"L Joystick", lJoystick,
			"R Joystick", rJoystick
		};

		MiscModel::drawFlags("Controller", lf, readOnly);
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
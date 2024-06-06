#include "MiscModel.hpp"

#include "InputWrapper.hpp"
#include "Ui.hpp"

#include <array>

namespace MiscModel
{
	void drawEnableButton(const char* label, const char* button, bool* flag)
	{
		Ui::labelXSpacing(label);
		if (Ui::buttonItemWidth(button))
		{
			*flag = true;
		}
	}

	void drawFlags(const char* label, std::span<const Ui::LabelFlag> lf, bool readOnly)
	{
		Ui::labelXSpacing(label);
		Ui::buttonsFlags(lf, readOnly);
	}

	void drawMiscPauseGame(bool* pauseGame, bool readOnly)
	{
		const Ui::LabelFlag lf{ "Pause Game", pauseGame };
		MiscModel::drawFlags("Misc", { &lf, 1 }, readOnly);
	}

	void drawMiscHideHud(bool* hideHud, bool readOnly)
	{
		const Ui::LabelFlag lf{ "Hide Hud", hideHud };
		MiscModel::drawFlags("Misc", { &lf, 1 }, readOnly);
	}

	void drawMiscPauseGameHideHud(bool* pauseGame, bool* hideHud, bool readOnly)
	{
		const std::array<Ui::LabelFlag, 2> lf
		{
			"Pause Game", pauseGame,
			"Hide Hud", hideHud
		};

		MiscModel::drawFlags("Misc", lf, readOnly);
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

	void updateTimescale(InputWrapper* input, s32 it, s32 dt, float* timescale, float speed, float min, float max)
	{
		if (input->isPressed(it))
		{
			*timescale = std::clamp(*timescale * speed, min, max);
		}
		if (input->isPressed(dt))
		{
			*timescale = std::clamp(*timescale / speed, min, max);
		}
	}

	void teleportToCamera(InputWrapper* input, s32 id, bool* flag)
	{
		if (input->isHeld(id))
		{
			*flag = true;
		}
	}

	void toggle(InputWrapper* input, s32 id, bool* flag)
	{
		if (input->isPressed(id))
		{
			*flag = !*flag;
		}
	}
}
#pragma once

#include <SDL.h>

namespace Gamepad
{
	enum class Layout
	{
		Xbox,
		PlayStation,
		Nintendo
	};

	enum class Action
	{
		Up,
		Down,
		Left,
		Right,
		Start,
		Select,
		L3,
		R3,
		L1,
		R1,
		L2,
		R2,
		Cross,
		Circle,
		Square,
		Triangle,
		LeftStickXPos,
		LeftStickXNeg,
		LeftStickYPos,
		LeftStickYNeg,
		RightStickXPos,
		RightStickXNeg,
		RightStickYPos,
		RightStickYNeg,
		Misc, // Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button
		Touchpad,
		Count,
	};

	const char* toString(Gamepad::Layout layout, Gamepad::Action action);
	Gamepad::Layout layoutFromSDLControllerType(SDL_GameControllerType type);
};
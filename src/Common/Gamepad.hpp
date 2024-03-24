#pragma once

namespace Gamepad
{
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
		Count,
	};

	const char* toStringXbox(Gamepad::Action action);
	const char* toStringPlayStation(Gamepad::Action button);
};
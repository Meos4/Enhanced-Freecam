#pragma once

#include "Gamepad.hpp"
#include "Types.hpp"

namespace GamepadWrapper
{
	inline constexpr u32 gamepadIdMin{ 0 };
	inline constexpr u32 gamepadMax{ 4 };

	void update();
	const char* name(u32 id);
	bool isPressed(Gamepad::Action action, u32 id);
	bool isHeld(Gamepad::Action action, u32 id);
	float sensitivity(Gamepad::Action action, u32 id);
}
#pragma once

#include "Common/Keyboard.hpp"
#include "Common/Types.hpp"

namespace Windows::Keyboard
{
	bool isHeld(u8 key);
	::Keyboard::Layout layout();
};
#include "Keyboard.hpp"

#include "Header.hpp"

namespace Windows::Keyboard
{
	bool isHeld(u8 key)
	{
		return GetAsyncKeyState(key) & 0x8000 ? true : false;
	}
};
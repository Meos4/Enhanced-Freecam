#include "Keyboard.hpp"

#include "Header.hpp"

namespace Windows::Keyboard
{
	bool isHeld(u8 key)
	{
		return GetAsyncKeyState(key) & 0x8000 ? true : false;
	}

	::Keyboard::Layout layout()
	{
		enum : DWORD
		{
			Azerty_French = 0x040C,
			Azerty_Belgium = 0x080C,
			Azerty_Canadian = 0x0C0C,
			Azerty_Swiss = 0x100C
		};

		switch (LOWORD(GetKeyboardLayout(0)))
		{
		case Azerty_French:
		case Azerty_Belgium:
		case Azerty_Canadian:
		case Azerty_Swiss:
			return ::Keyboard::Layout::Azerty;
		default: return ::Keyboard::Layout::Qwerty;
		}
	}
};
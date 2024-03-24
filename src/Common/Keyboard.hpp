#pragma once

#include "Types.hpp"

namespace Keyboard
{
	using Key_t = u8;

	enum class Key : Key_t
	{
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,

		NumLock,
		Numpad0,
		Numpad1,
		Numpad2,
		Numpad3,
		Numpad4,
		Numpad5,
		Numpad6,
		Numpad7,
		Numpad8,
		Numpad9,
		NumMultiply,
		NumAdd,
		NumSubtract,
		NumDecimal,
		NumDivide,

		Esc,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		Snapshot,
		Pause,

		Backspace,
		Enter,
		LShift,
		RShift,
		Tab,
		LCtrl,
		RCtrl,
		LAlt,
		RAlt,
		CapsLock,
		Space,
		Select,
		LSystem,
		RSystem,
		Sleep,
		Plus,
		Comma,
		Minus,
		Period,
		Slash,

		Insert,
		Home,
		PageUp,
		PageDown,
		Delete,
		End,

		Left,
		Up,
		Right,
		Down,
		
		// Layout
		Qwerty_Semicolon,
		Qwerty_Tilde,
		Qwerty_LBracket,
		Qwerty_Backslash,
		Qwerty_RBracket,
		Qwerty_Apostrophe,

		Count,

		Azerty_Dollar = Qwerty_Semicolon,
		Azerty_Percent = Qwerty_Tilde,
		Azerty_RBracket = Qwerty_LBracket,
		Azerty_Multiply = Qwerty_Backslash,
		Azerty_Circumflex = Qwerty_RBracket,
		Azerty_Square = Qwerty_Apostrophe
	};

	void update();
	bool isPressed(Keyboard::Key key);
	bool isHeld(Keyboard::Key key);
	const char* toStringQwerty(Keyboard::Key key);
	const char* toStringAzerty(Keyboard::Key key);
	Keyboard::Key qwertyToAzertyKey(Keyboard::Key key);
};
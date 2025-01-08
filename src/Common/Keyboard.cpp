#include "Keyboard.hpp"

#include <array>
#include <unordered_set>
#include <utility>

#if _WIN32
#include "Windows/Header.hpp"
#include "Windows/Keyboard.hpp"

static constexpr std::array<u8, static_cast<std::size_t>(Keyboard::Key::Count)> platformKeys
{
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5A,

	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39,

	VK_NUMLOCK, VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2,
	VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6,
	VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_MULTIPLY,
	VK_ADD, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE,

	VK_ESCAPE, VK_F1, VK_F2, VK_F3,
	VK_F4, VK_F5, VK_F6, VK_F7,
	VK_F8, VK_F9, VK_F10, VK_F11,
	VK_F12,

	VK_SNAPSHOT, VK_PAUSE,

	VK_BACK, VK_RETURN, VK_LSHIFT, VK_RSHIFT,
	VK_TAB, VK_LCONTROL, VK_RCONTROL, VK_LMENU,
	VK_RMENU, VK_CAPITAL, VK_SPACE, VK_SELECT,
	VK_LWIN, VK_RWIN, VK_SLEEP, VK_OEM_PLUS,
	VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_2,

	VK_INSERT, VK_HOME, VK_PRIOR, VK_NEXT,
	VK_DELETE, VK_END,

	VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN,

	VK_OEM_1, VK_OEM_3, VK_OEM_4, VK_OEM_5,
	VK_OEM_6, VK_OEM_7
};

namespace OsKeyboard = Windows::Keyboard;
#endif

namespace Keyboard
{
	static std::unordered_set<Keyboard::Key> usedKeys{}, usedKeysWaitingUpdate{};
	static Keyboard::Layout currentLayout{ Keyboard::Layout::Qwerty };

	static const char* toStringQwerty(Keyboard::Key key)
	{
		static constexpr std::array<const char*, static_cast<std::size_t>(Keyboard::Key::Count)> names
		{
			"A", "B", "C", "D", "E", "F", "G", "H",
			"I", "J", "K", "L", "M", "N", "O", "P",
			"Q", "R", "S", "T", "U", "V", "W", "X",
			"Y", "Z",

			"0", "1", "2", "3", "4", "5", "6", "7",
			"8", "9",

			"Num Lock", "Numpad 0", "Numpad 1", "Numpad 2",
			"Numpad 3", "Numpad 4", "Numpad 5", "Numpad 6",
			"Numpad 7", "Numpad 8", "Numpad 9", "Num *",
			"Num +", "Num -", "Num .", "Num /",

			"Escape", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
			"F8", "F9", "F10", "F11", "F12",

			"Snapshot", "Pause",

			"Backspace", "Enter", "Left Shift", "Right Shift",
			"Tab", "Left Ctrl", "Right Ctrl", "Left Alt",
			"Right Alt", "Caps lock", "Space", "Select",
			"Left System", "Right System", "Sleep", "+",
			",", "-", ".", "/",

			"Insert", "Home", "Page Up", "Page Down",
			"Delete", "End",

			"Left", "Up", "Right", "Down",

			";", "~", "[", "\\",
			"]", "\'"
		};

		return names[static_cast<std::size_t>(key)];
	}

	static const char* toStringAzerty(Keyboard::Key key)
	{
		switch (key)
		{
		case Keyboard::Key::Azerty_Dollar: return "$";
		case Keyboard::Key::Azerty_Percent: return "%";
		case Keyboard::Key::Azerty_RBracket: return "]";
		case Keyboard::Key::Azerty_Multiply: return "*";
		case Keyboard::Key::Azerty_Circumflex: return "^";
		case Keyboard::Key::Azerty_Square: return "Square";
		}

		return Keyboard::toStringQwerty(key);
	}

	void update()
	{
		usedKeys = std::move(usedKeysWaitingUpdate);
		usedKeysWaitingUpdate.clear();
		currentLayout = OsKeyboard::layout();
	}

	bool isPressed(Keyboard::Key key)
	{
		if (Keyboard::isHeld(key))
		{
			return !usedKeys.contains(key);
		}
		return false;
	}

	bool isHeld(Keyboard::Key key)
	{
		if (OsKeyboard::isHeld(platformKeys[static_cast<std::size_t>(key)]))
		{
			usedKeysWaitingUpdate.insert(key);
			return true;
		}
		return false;
	}

	Keyboard::Layout layout()
	{
		return currentLayout;
	}

	const char* toString(Keyboard::Layout layout, Keyboard::Key key)
	{
		switch (layout)
		{
		case Keyboard::Layout::Azerty: return Keyboard::toStringAzerty(key);
		default: return Keyboard::toStringQwerty(key);
		}
	}

	Keyboard::Key qwertyToAzertyKey(Keyboard::Key key)
	{
		switch (key)
		{
		case Keyboard::Key::Q: return Keyboard::Key::A;
		case Keyboard::Key::W: return Keyboard::Key::Z;
		case Keyboard::Key::A: return Keyboard::Key::Q;
		case Keyboard::Key::Qwerty_Semicolon: return Keyboard::Key::M;
		case Keyboard::Key::Qwerty_Apostrophe: return Keyboard::Key::Azerty_Percent;
		case Keyboard::Key::Z: return Keyboard::Key::W;
		case Keyboard::Key::M: return Keyboard::Key::Comma;
		}

		return key;
	}
};
#include "Gamepad.hpp"

#include <array>

namespace Gamepad
{
	static const char* toStringXbox(Gamepad::Action action)
	{
		static constexpr std::array<const char*, static_cast<std::size_t>(Gamepad::Action::Count)> names
		{
			"D-Pad Up", "D-Pad Down", "D-Pad Left", "D-Pad Right",
			"Start", "Back", "Left Thumb", "Right Thumb",
			"Left Shoulder", "Right Shoulder", "Left Trigger", "Right Trigger",
			"(A)", "(B)", "(X)", "(Y)",
			"Left Thumb X+", "Left Thumb X-", "Left Thumb Y+", "Left Thumb Y-",
			"Right Thumb X+", "Right Thumb X-", "Right Thumb Y+", "Right Thumb Y-",
			"Share", "Touchpad"
		};

		return names[static_cast<std::size_t>(action)];
	}

	static const char* toStringPlayStation(Gamepad::Action action)
	{
		switch (action)
		{
		case Gamepad::Action::Select: return "Select";
		case Gamepad::Action::L1: return "L1";
		case Gamepad::Action::R1: return "R1";
		case Gamepad::Action::L3: return "L3";
		case Gamepad::Action::R3: return "R3";
		case Gamepad::Action::L2: return "L2";
		case Gamepad::Action::R2: return "R2";
		case Gamepad::Action::Cross: return "Cross";
		case Gamepad::Action::Circle: return "Circle";
		case Gamepad::Action::Square: return "Square";
		case Gamepad::Action::Triangle: return "Triangle";
		case Gamepad::Action::Misc: return "Microphone";
		}

		return Gamepad::toStringXbox(action);
	}

	static const char* toStringNintendo(Gamepad::Action action)
	{
		switch (action)
		{
		case Gamepad::Action::Cross: return "(B)";
		case Gamepad::Action::Circle: return "(A)";
		case Gamepad::Action::Square: return "(Y)";
		case Gamepad::Action::Triangle: return "(X)";
		case Gamepad::Action::Misc: return "Capture";
		}

		return Gamepad::toStringXbox(action);
	}

	const char* toString(Gamepad::Layout layout, Gamepad::Action action)
	{
		switch (layout)
		{
		case Gamepad::Layout::PlayStation: return Gamepad::toStringPlayStation(action);
		case Gamepad::Layout::Nintendo: return Gamepad::toStringNintendo(action);
		default: return Gamepad::toStringXbox(action);
		}
	}

	Gamepad::Layout layoutFromSDLControllerType(SDL_GameControllerType type)
	{
		switch (type)
		{
		case SDL_CONTROLLER_TYPE_PS3:
		case SDL_CONTROLLER_TYPE_PS4:
		case SDL_CONTROLLER_TYPE_PS5:
			return Gamepad::Layout::PlayStation;
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_LEFT:
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_RIGHT:
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_JOYCON_PAIR:
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
			return Gamepad::Layout::Nintendo;
		default: return Gamepad::Layout::Xbox;
		}
	}
}
#include "GamepadWrapper.hpp"

#include "SDL/Gamepad.hpp"

#include "Settings.hpp"

#include <unordered_set>

namespace GamepadWrapper
{
	static std::unordered_set<Gamepad::Action> usedActions{}, usedActionsWaitingUpdate{};

	static float triggerVal(float val)
	{
		return val >= g_settings.gamepad.triggerDeadzone ? val : 0.f;
	}

	static float stickVal(float val)
	{
		return val >= g_settings.gamepad.stickDeadzone ? val : 0.f;
	}

	static SDL::Gamepad::Button_t toSDLButton(Gamepad::Action action)
	{
		switch (action)
		{
		case Gamepad::Action::Up: return SDL_CONTROLLER_BUTTON_DPAD_UP;
		case Gamepad::Action::Down: return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
		case Gamepad::Action::Left: return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
		case Gamepad::Action::Right: return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
		case Gamepad::Action::Start: return SDL_CONTROLLER_BUTTON_START;
		case Gamepad::Action::Select: return SDL_CONTROLLER_BUTTON_BACK;
		case Gamepad::Action::L3: return SDL_CONTROLLER_BUTTON_LEFTSTICK;
		case Gamepad::Action::R3: return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
		case Gamepad::Action::L1: return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
		case Gamepad::Action::R1: return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
		case Gamepad::Action::Triangle: return SDL_CONTROLLER_BUTTON_Y;
		case Gamepad::Action::Circle: return SDL_CONTROLLER_BUTTON_B;
		case Gamepad::Action::Cross: return SDL_CONTROLLER_BUTTON_A;
		case Gamepad::Action::Square: return SDL_CONTROLLER_BUTTON_X;
		case Gamepad::Action::Misc: return SDL_CONTROLLER_BUTTON_MISC1;
		case Gamepad::Action::Touchpad: return SDL_CONTROLLER_BUTTON_TOUCHPAD;
		};

		return SDL_CONTROLLER_BUTTON_INVALID;
	}

	static float toSDLAxisMotion(Gamepad::Action action, u32 id)
	{
		auto getPos = [](float val) { return val > 0.f ? val : 0.f; };
		auto getNeg = [](float val) { return val < 0.f ? -val : 0.f; };

		switch (action)
		{
		case Gamepad::Action::L2: return GamepadWrapper::triggerVal(SDL::Gamepad::leftTrigger(id));
		case Gamepad::Action::R2: return GamepadWrapper::triggerVal(SDL::Gamepad::rightTrigger(id));
		case Gamepad::Action::LeftStickXPos: return GamepadWrapper::stickVal(getPos(SDL::Gamepad::leftStick(id).x));
		case Gamepad::Action::LeftStickXNeg: return GamepadWrapper::stickVal(getNeg(SDL::Gamepad::leftStick(id).x));
		case Gamepad::Action::LeftStickYPos: return GamepadWrapper::stickVal(getPos(SDL::Gamepad::leftStick(id).y));
		case Gamepad::Action::LeftStickYNeg: return GamepadWrapper::stickVal(getNeg(SDL::Gamepad::leftStick(id).y));
		case Gamepad::Action::RightStickXPos: return GamepadWrapper::stickVal(getPos(SDL::Gamepad::rightStick(id).x));
		case Gamepad::Action::RightStickXNeg: return GamepadWrapper::stickVal(getNeg(SDL::Gamepad::rightStick(id).x));
		case Gamepad::Action::RightStickYPos: return GamepadWrapper::stickVal(getPos(SDL::Gamepad::rightStick(id).y));
		case Gamepad::Action::RightStickYNeg: return GamepadWrapper::stickVal(getNeg(SDL::Gamepad::rightStick(id).y));
		}

		return 0.f;
	}

	void update()
	{
		GamepadWrapper::usedActions = std::move(GamepadWrapper::usedActionsWaitingUpdate);
		GamepadWrapper::usedActionsWaitingUpdate.clear();
	}

	const char* name(u32 id)
	{
		return SDL::Gamepad::name(id);
	}

	bool isPressed(Gamepad::Action action, u32 id)
	{
		if (GamepadWrapper::isHeld(action, id))
		{
			return !GamepadWrapper::usedActions.contains(action);
		}

		return false;
	}

	bool isHeld(Gamepad::Action action, u32 id)
	{
		const auto sdlButton{ GamepadWrapper::toSDLButton(action) };

		if ((sdlButton != SDL_CONTROLLER_BUTTON_INVALID && SDL::Gamepad::isHeld(id, sdlButton)) ||
			GamepadWrapper::toSDLAxisMotion(action, id) != 0.f)
		{
			GamepadWrapper::usedActionsWaitingUpdate.insert(action);
			return true;
		}

		return false;
	}

	float GamepadWrapper::sensitivity(Gamepad::Action action, u32 id)
	{
		const auto sensitivity{ GamepadWrapper::toSDLAxisMotion(action, id) };

		if (sensitivity)
		{
			return sensitivity;
		}

		if (GamepadWrapper::isHeld(action, id))
		{
			return 1.f;
		}

		return sensitivity;
	}
}
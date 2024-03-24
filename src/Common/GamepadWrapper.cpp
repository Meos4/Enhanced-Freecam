#include "GamepadWrapper.hpp"

#if _WIN32
#include "Windows/XInputGamepad.hpp"
#endif

#include "Settings.hpp"

#include <array>
#include <unordered_set>

namespace GamepadWrapper
{
	static std::unordered_set<Gamepad::Action> usedActions{}, usedActionsWaitingUpdate{};
	static std::array<bool, GamepadWrapper::gamepadMax> activeGamepads;

	static float triggerVal(float val)
	{
		return val >= g_settings.gamepad.triggerDeadzone ? val : 0.f;
	}

	static float stickVal(float val)
	{
		return val >= g_settings.gamepad.stickDeadzone ? val : 0.f;
	}

#if _WIN32
	static constexpr auto xInputMin{ Windows::XInputGamepad::gamepadIdMin };
	static std::array<Windows::XInputGamepad, Windows::XInputGamepad::gamepadMax> xInputGamepads
	{
		xInputMin, xInputMin + 1, xInputMin + 2, xInputMin + 3
	};

	static Windows::XInputGamepad::Button_t toXInputButton(Gamepad::Action action)
	{
		switch (action)
		{
		case Gamepad::Action::Up: return Windows::XInputGamepad::Button::Up;
		case Gamepad::Action::Down: return Windows::XInputGamepad::Button::Down;
		case Gamepad::Action::Left: return Windows::XInputGamepad::Button::Left;
		case Gamepad::Action::Right: return Windows::XInputGamepad::Button::Right;
		case Gamepad::Action::Start: return Windows::XInputGamepad::Button::Start;
		case Gamepad::Action::Select: return Windows::XInputGamepad::Button::Back;
		case Gamepad::Action::L3: return Windows::XInputGamepad::Button::Left_Thumb;
		case Gamepad::Action::R3: return Windows::XInputGamepad::Button::Right_Thumb;
		case Gamepad::Action::L1: return Windows::XInputGamepad::Button::Left_Shoulder;
		case Gamepad::Action::R1: return Windows::XInputGamepad::Button::Right_Shoulder;
		case Gamepad::Action::Triangle: return Windows::XInputGamepad::Button::Y;
		case Gamepad::Action::Circle: return Windows::XInputGamepad::Button::B;
		case Gamepad::Action::Cross: return Windows::XInputGamepad::Button::A;
		case Gamepad::Action::Square: return Windows::XInputGamepad::Button::X;
		};

		return 0;
	}

	static float xInputSensitivityButton(Gamepad::Action action, const Windows::XInputGamepad& gamepad)
	{
		auto getPos = [](float val) { return val > 0.f ? val : 0.f; };
		auto getNeg = [](float val) { return val < 0.f ? -val : 0.f; };

		switch (action)
		{
		case Gamepad::Action::L2: return GamepadWrapper::triggerVal(gamepad.leftTrigger());
		case Gamepad::Action::R2: return GamepadWrapper::triggerVal(gamepad.rightTrigger());
		case Gamepad::Action::LeftStickXPos: return GamepadWrapper::stickVal(getPos(gamepad.leftStick().x));
		case Gamepad::Action::LeftStickXNeg: return GamepadWrapper::stickVal(getNeg(gamepad.leftStick().x));
		case Gamepad::Action::LeftStickYPos: return GamepadWrapper::stickVal(getPos(gamepad.leftStick().y));
		case Gamepad::Action::LeftStickYNeg: return GamepadWrapper::stickVal(getNeg(gamepad.leftStick().y));
		case Gamepad::Action::RightStickXPos: return GamepadWrapper::stickVal(getPos(gamepad.rightStick().x));
		case Gamepad::Action::RightStickXNeg: return GamepadWrapper::stickVal(getNeg(gamepad.rightStick().x));
		case Gamepad::Action::RightStickYPos: return GamepadWrapper::stickVal(getPos(gamepad.rightStick().y));
		case Gamepad::Action::RightStickYNeg: return GamepadWrapper::stickVal(getNeg(gamepad.rightStick().y));
		}

		return 0.f;
	}
#endif

	void update()
	{
		for (auto& active : GamepadWrapper::activeGamepads)
		{
			active = false;
		}

	#if _WIN32
		for (std::size_t i{}; i < GamepadWrapper::xInputGamepads.size(); ++i)
		{
			if (GamepadWrapper::xInputGamepads[i].update())
			{
				GamepadWrapper::activeGamepads[i] = true;
			}
		}
	#endif

		GamepadWrapper::usedActions = std::move(GamepadWrapper::usedActionsWaitingUpdate);
		GamepadWrapper::usedActionsWaitingUpdate.clear();
	}

	bool isActive(u32 id)
	{
		return GamepadWrapper::activeGamepads[id];
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
	#if _WIN32
		const auto xInputButton{ GamepadWrapper::toXInputButton(action) };
		const auto& xInputPad{ GamepadWrapper::xInputGamepads[id] };

		if ((xInputButton && xInputPad.isHeld(xInputButton)) || GamepadWrapper::xInputSensitivityButton(action, xInputPad) != 0.f)
		{
			GamepadWrapper::usedActionsWaitingUpdate.insert(action);
			return true;
		}
	#endif

		return false;
	}

	float GamepadWrapper::sensitivity(Gamepad::Action action, u32 id)
	{
		auto sensitivity{ 0.f };

	#if _WIN32
		sensitivity = GamepadWrapper::xInputSensitivityButton(action, GamepadWrapper::xInputGamepads[id]);

		if (sensitivity)
		{
			return sensitivity;
		}
	#endif

		if (GamepadWrapper::isHeld(action, id))
		{
			sensitivity = 1.f;
		}

		return sensitivity;
	}
}
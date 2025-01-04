#include "Gamepad.hpp"

#include "Common/GamepadWrapper.hpp"
#include "Common/Math.hpp"

#include <algorithm>
#include <array>

namespace SDL::Gamepad
{
	static constexpr SDL_JoystickID invalidId{ -1 };

	struct Pad
	{
		SDL_GameController* controller{ nullptr };
		SDL_JoystickID id{ SDL::Gamepad::invalidId };
		s32 heldBinaryButtons;
		Vec2<float> leftStick;
		Vec2<float> rightStick;
		float leftTrigger;
		float rightTrigger;
	};
	
	static constexpr auto maxBinaryButtons{ sizeof(decltype(SDL::Gamepad::Pad::heldBinaryButtons)) * 8 };
	static_assert(maxBinaryButtons >= SDL_CONTROLLER_BUTTON_MAX);

	static std::array<SDL::Gamepad::Pad, GamepadWrapper::gamepadMax> gamepads{};

	static SDL::Gamepad::Pad* createPad(s32 index)
	{
		if (index < gamepads.size())
		{
			const auto sdlId{ SDL_JoystickGetDeviceInstanceID(index) };

			if (sdlId != -1)
			{
				for (auto& gamepad : gamepads)
				{
					if (!gamepad.controller)
					{
						gamepad.controller = SDL_GameControllerOpen(index);
						if (!gamepad.controller)
						{
							return nullptr;
						}

						gamepad.id = sdlId;
						return &gamepad;
					}
				}
			}
		}

		return nullptr;
	}

	static SDL::Gamepad::Pad* pad(SDL_JoystickID id)
	{
		for (auto& gamepad : gamepads)
		{
			if (gamepad.id == id)
			{
				return &gamepad;
			}
		}

		for (s32 i{}; i < gamepads.size(); ++i)
		{
			if (!gamepads[i].controller)
			{
				return SDL::Gamepad::createPad(i);
			}
		}

		return nullptr;
	}

	void handleButtonDown(const SDL_Event& e)
	{
		auto* const pad{ SDL::Gamepad::pad(e.cbutton.which) };
		if (pad)
		{
			pad->heldBinaryButtons |= (1 << e.cbutton.button);
		}
	}

	void handleButtonUp(const SDL_Event& e)
	{
		auto* const pad{ SDL::Gamepad::pad(e.cbutton.which) };
		if (pad)
		{
			pad->heldBinaryButtons &= ~(1 << e.cbutton.button);
		}
	}

	void handleAxisMotion(const SDL_Event& e)
	{
		auto* const pad{ SDL::Gamepad::pad(e.jaxis.which) };
		if (pad)
		{
			const auto value{ Math::normalizeInt<float>(e.jaxis.value) };
			
			switch (e.jaxis.axis)
			{
			case 0: pad->leftStick.x = value; break;
			case 1: pad->leftStick.y = (value * -1.f); break;
			case 2: pad->rightStick.x = value; break;
			case 3: pad->rightStick.y = (value * -1.f); break;
			case 4: pad->leftTrigger = (value + 1.f) / 2.f; break;
			case 5: pad->rightTrigger = (value + 1.f) / 2.f; break;
			}
		}
	}

	void handleConnection(const SDL_Event& e)
	{
		SDL::Gamepad::createPad(e.jdevice.which);
	}

	void handleDisconnect(const SDL_Event& e)
	{
		const auto pad{ std::find_if(gamepads.begin(), gamepads.end(), [&e](const auto& pad)
		{
			return pad.id == e.jdevice.which;
		})};

		if (pad != gamepads.end())
		{
			SDL_GameControllerClose(pad->controller);
			*pad = {};

			for (std::size_t i{}; i < gamepads.size() - 1; ++i)
			{
				auto* const current{ &gamepads[i] };
				auto* const next{ &gamepads[i + 1] };

				if (!current->controller && next->controller)
				{
					*current = *next;
					*next = {};
				}
			}
		}
	}

	bool isHeld(u32 id, SDL::Gamepad::Button_t button)
	{
		return gamepads[id].heldBinaryButtons & (1 << button) ? true : false;
	}

	Vec2<float> leftStick(u32 id)
	{
		return gamepads[id].leftStick;
	}

	Vec2<float> rightStick(u32 id)
	{
		return gamepads[id].rightStick;
	}

	float leftTrigger(u32 id)
	{
		return gamepads[id].leftTrigger;
	}

	float rightTrigger(u32 id)
	{
		return gamepads[id].rightTrigger;
	}

	const char* name(u32 id)
	{
		return SDL_JoystickName(SDL_JoystickFromInstanceID(gamepads[id].id));
	}
}
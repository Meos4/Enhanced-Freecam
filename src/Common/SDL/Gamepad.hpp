#pragma once

#include "Common/Types.hpp"
#include "Common/Vec2.hpp"

#include <SDL.h>

namespace SDL::Gamepad
{
	using Button_t = decltype(SDL_ControllerButtonEvent::button);

	void handleButtonDown(const SDL_Event& e);
	void handleButtonUp(const SDL_Event& e);
	void handleAxisMotion(const SDL_Event& e);
	void handleConnection(const SDL_Event& e);
	void handleDisconnect(const SDL_Event& e);
	bool isHeld(u32 id, SDL::Gamepad::Button_t button);
	Vec2<float> leftStick(u32 id);
	Vec2<float> rightStick(u32 id);
	float leftTrigger(u32 id);
	float rightTrigger(u32 id);
	const char* name(u32 id);
	SDL_GameControllerType type(u32 id);
}
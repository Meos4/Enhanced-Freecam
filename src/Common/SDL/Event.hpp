#pragma once

#include <SDL.h>

namespace SDL::Event
{
	using Init_t = decltype(SDL_INIT_VIDEO);
	
    void init(Init_t v);
	void update();
}
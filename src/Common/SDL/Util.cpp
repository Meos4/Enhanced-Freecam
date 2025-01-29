#include "Util.hpp"

#include <SDL.h>

namespace SDL::Util
{
	s32 nbMonitors()
	{
		return SDL_GetNumVideoDisplays();
	}

	std::optional<Vec2<s32>> resolution(s32 monitor)
	{
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(monitor, &mode) == 0)
		{
			return Vec2<s32>{ mode.w, mode.h };
		}

		return std::nullopt;
	}
}
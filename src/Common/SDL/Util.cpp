#include "Util.hpp"

#include <SDL.h>

namespace SDL::Util
{
	s32 nbMonitors()
	{
		return SDL_GetNumVideoDisplays();
	}
}
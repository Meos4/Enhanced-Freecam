#include "Event.hpp"

#include "Renderer.hpp"

#include "imgui_impl_sdl2.h"

#include <SDL.h>

namespace SDL::Event
{
	void update()
	{
		SDL_Event event;

		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			switch (event.type)
			{
			case SDL_QUIT:
			{
				SDL::Renderer::setShouldClose();
				break;
			}

			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
					event.window.windowID == SDL_GetWindowID(SDL::Renderer::window()))
				{
					SDL::Renderer::setShouldClose();
				}
				break;
			}
			}
		}
	}
}
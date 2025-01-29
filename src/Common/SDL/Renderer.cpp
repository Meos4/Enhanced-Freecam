#include "Renderer.hpp"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_syswm.h>

namespace SDL::Renderer
{
	static struct
	{
		SDL_Window* window{ nullptr };
		SDL_GLContext context{ nullptr };
		float lastFrame{};
		float deltaTime{};
		bool shouldClose{};
	} v;

	void createWindow(const char* title, s32 width, s32 height)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
		SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		static constexpr auto flags{ SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI };
		v.window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

		v.context = SDL_GL_CreateContext(v.window);
		SDL_GL_MakeCurrent(v.window, v.context);

		SDL_GL_SetSwapInterval(1); // VSync

		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;

		ImGui_ImplSDL2_InitForOpenGL(v.window, v.context);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	SDL_Window* window()
	{
		return v.window;
	}

	void* platformWindow()
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		return SDL_GetWindowWMInfo(v.window, &wmInfo) == SDL_TRUE ? wmInfo.info.win.window : nullptr;
	}

	float time()
	{
		return static_cast<float>(SDL_GetTicks64()) / 1000.0f;
	}

	Vec2<s32> windowDimension()
	{
		s32 width, height;
    	SDL_GetWindowSize(v.window, &width, &height);
		return { width, height };
	}

	void beginRender()
	{
		const auto now{ SDL::Renderer::time() };
		v.deltaTime = now - v.lastFrame;
		v.lastFrame = now;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
	}

	void endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop)
	{
		ImGui::Render();
		const auto [w, h]{ SDL::Renderer::windowDimension() };
		glViewport(0, 0, w, h);
		glClearColor(rgb[0], rgb[1], rgb[2], 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			if (viewportsAlwaysOnTop)
			{
				const auto* const imGuiCurrentContext{ ImGui::GetCurrentContext() };
				for (s32 i{ 1 }; i < imGuiCurrentContext->Viewports.Size; ++i)
				{
					auto* const viewport{ imGuiCurrentContext->Viewports[i] };
					if (!viewport->PlatformWindowCreated)
					{
						viewport->Flags |= ImGuiViewportFlags_TopMost;
					}
				}
			}

			auto* const currentWindow{ SDL_GL_GetCurrentWindow() };
			const auto currentContext{ SDL_GL_GetCurrentContext() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(currentWindow, currentContext);
		}

		SDL_GL_SwapWindow(v.window);
	}

	void destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		SDL_GL_DeleteContext(v.context);
		SDL_DestroyWindow(v.window);
		SDL_Quit();
	}

	bool shouldClose()
	{
		return v.shouldClose;
	}

	s32 nbMonitors()
	{
		return SDL_GetNumVideoDisplays();
	}

	std::optional<s32> refreshRate(s32 monitor)
	{
		SDL_DisplayMode mode;
		if (SDL_GetCurrentDisplayMode(monitor, &mode) == 0)
		{
			return mode.refresh_rate;
		}

		return std::nullopt;
	}

	float deltaTime()
	{
		return v.deltaTime;
	}

	void setWindowDimension(s32 width, s32 height)
	{
		SDL_SetWindowSize(v.window, width, height);
	}

	void setShouldClose()
	{
		v.shouldClose = true;
	}
}
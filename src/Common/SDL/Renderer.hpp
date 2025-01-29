#pragma once

#include "Common/Types.hpp"
#include "Common/Vec2.hpp"

#include <array>
#include <optional>

struct SDL_Window;

namespace SDL::Renderer
{
	void createWindow(const char* title, s32 width, s32 height);
	SDL_Window* window();
	void* platformWindow();
	float time();
	Vec2<s32> windowDimension();
	void beginRender();
	void endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop);
	void destroy();
	bool shouldClose();
	std::optional<s32> refreshRate();
	float deltaTime();
	void setWindowDimension(s32 width, s32 height);
	void setShouldClose();
}
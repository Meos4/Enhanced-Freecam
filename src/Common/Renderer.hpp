#pragma once

#include "Types.hpp"
#include "Vec2.hpp"

#include <array>
#include <optional>

namespace Renderer
{
	void* platformWindow();
	void createWindow(const char* name, s32 width, s32 height);
	void beginRender();
	void endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop);
	bool shouldClose();
	void destroy();
	s32 nbMonitors();
	std::optional<s32> refreshRate(s32 monitor);
	float time();
	float deltaTime();
	Vec2<s32> windowDimension();
	void setWindowDimension(s32 width, s32 height);
}
#include "Renderer.hpp"

#include "GLFW/Renderer.hpp"

namespace Renderer
{
	static GLFW::Renderer* renderer{ nullptr };

	void* platformWindow()
	{
		return renderer->platformWindow();
	}

	void createWindow(const char* name, s32 width, s32 height)
	{
		if (renderer)
		{
			destroy();
		}

		renderer = new GLFW::Renderer{ name, width, height };
	}

	void beginRender()
	{
		renderer->beginRender();
	}

	void endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop)
	{
		renderer->endRender(rgb, viewportsAlwaysOnTop);
	}

	bool shouldClose()
	{
		return renderer->shouldClose();
	}

	void destroy()
	{
		renderer->destroy();
		delete renderer;
	}

	s32 nbMonitors()
	{
		return renderer->nbMonitors();
	}

	std::optional<s32> refreshRate(s32 monitor)
	{
		return renderer->refreshRate(monitor);
	}

	float time()
	{
		return renderer->time();
	}

	float deltaTime()
	{
		return renderer->deltaTime();
	}

	Vec2<s32> windowDimension()
	{
		return renderer->windowDimension();
	}

	void setWindowDimension(s32 width, s32 height)
	{
		renderer->setWindowDimension(width, height);
	}
}
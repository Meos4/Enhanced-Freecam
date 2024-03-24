#pragma once

#include "Common/Types.hpp"
#include "Common/Vec2.hpp"

#include <array>
#include <optional>

struct GLFWwindow;

namespace GLFW
{
	class Renderer final
	{
	public:
		Renderer(const char* title, s32 width, s32 height);

		void* platformWindow() const;
		void beginRender();
		void endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop);
		void destroy();
		bool shouldClose() const;
		s32 nbMonitors() const;
		std::optional<s32> refreshRate(s32 monitor) const;
		float time() const;

		float deltaTime() const;
		Vec2<s32> windowDimension() const;

		void setWindowDimension(s32 width, s32 height);
	private:
		GLFWwindow* m_window;
		s32 m_width;
		s32 m_height;
		float m_lastFrame{};
		float m_deltaTime{};
	};
}
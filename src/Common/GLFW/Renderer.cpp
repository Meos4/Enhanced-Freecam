#include "Renderer.hpp"

#if _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

namespace GLFW
{
	Renderer::Renderer(const char* title, s32 width, s32 height)
		: m_width(width), m_height(height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSwapInterval(1); // VSync
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
	}

	void* Renderer::platformWindow() const
	{
#if _WIN32
		return glfwGetWin32Window(m_window);
#endif
	}

	void Renderer::beginRender()
	{
		const auto now{ time() };
		m_deltaTime = now - m_lastFrame;
		m_lastFrame = now;
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Renderer::endRender(const std::array<float, 3>& rgb, bool viewportsAlwaysOnTop)
	{
		ImGui::Render();
		glfwGetFramebufferSize(m_window, &m_width, &m_height);
		glViewport(0, 0, m_width, m_height);
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

			auto* const glfwCurrentContext{ glfwGetCurrentContext() };
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(glfwCurrentContext);
		}

		glfwSwapBuffers(m_window);
	}

	void Renderer::destroy()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	bool Renderer::shouldClose() const
	{
		return glfwWindowShouldClose(m_window);
	}

	s32 Renderer::nbMonitors() const
	{
		s32 nbMonitors{};
		glfwGetMonitors(&nbMonitors);
		return nbMonitors;
	}

	std::optional<s32> Renderer::refreshRate(s32 monitor) const
	{
		s32 nbMonitors{};
		auto** const monitors{ glfwGetMonitors(&nbMonitors) };

		if (nbMonitors > monitor)
		{
			auto* const videoModeMonitor{ glfwGetVideoMode(monitors[monitor]) };
			if (videoModeMonitor)
			{
				return videoModeMonitor->refreshRate;
			}
		}

		return std::nullopt;
	}

	float Renderer::time() const
	{
		return static_cast<float>(glfwGetTime());
	}

	float Renderer::deltaTime() const
	{
		return m_deltaTime;
	}

	Vec2<s32> Renderer::windowDimension() const
	{
		return { m_width, m_height };
	}

	void Renderer::setWindowDimension(s32 width, s32 height)
	{
		m_width = width;
		m_height = height;
		glfwSetWindowSize(m_window, width, height);
	}
}
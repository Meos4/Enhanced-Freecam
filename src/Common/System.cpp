#include "System.hpp"

#include "Debug/Performance.hpp"
#include "Debug/Sandbox.hpp"
#include "Debug/Ui.hpp"

#include "SDL/Event.hpp"

#if _WIN32
#include "Windows/Dwm.hpp"
#endif

#include "Console.hpp"
#include "Dockspace.hpp"
#include "GameInfo.hpp"
#include "GamepadWrapper.hpp"
#include "Json.hpp"
#include "Keyboard.hpp"
#include "Path.hpp"
#include "Renderer.hpp"
#include "Settings.hpp"
#include "Software.hpp"
#include "State.hpp"
#include "Welcome.hpp"

#include <filesystem>
#include <format>

namespace System
{
	static constexpr auto
		_System{ "System" },
		_Game{ "Game" },
		_Renderer{ "Renderer" },
		_width{ "width" },
		_height{ "height" };

	static void init()
	{
		const auto filename{ std::format("{}.json", _System) };
		const auto jsonRead{ Json::read(Path::settingsFile(filename.c_str())) };
		s32 width{ Software::width }, height{ Software::height };

		if (jsonRead.has_value() && jsonRead.value().contains(_Renderer))
		{
			const auto& jsonRenderer{ jsonRead.value()[_Renderer] };

			try
			{
				Json::get(jsonRenderer, _width, &width, 1);
				Json::get(jsonRenderer, _height, &height, 1);
			}
			catch (const Json::Exception& e)
			{
				Console::append(Console::Type::Exception, Json::exceptionFormat, _System, e.what());
			}
		}

		Renderer::createWindow(Software::name, width, height);
		g_settings.init();

		if (jsonRead.has_value())
		{
			const auto& json{ jsonRead.value() };
			g_settings.readSettings(json);
			State::readSettings(json);
		}

		Path::createSettingsDirectory();
	}

	static void destroy()
	{
		Json::Write json;

		g_settings.writeSettings(&json);
		State::writeSettings(&json);

		auto* const jsonRenderer{ &json[_Renderer] };
		const auto windowDimension{ Renderer::windowDimension() };
		Json::set(jsonRenderer, _width, windowDimension.x);
		Json::set(jsonRenderer, _height, windowDimension.y);

		const auto filename{ std::format("{}.json", _System) };
		Json::overwrite(json, Path::settingsFile(filename.c_str()));

		State::destroy();
		Renderer::destroy();
	}

	static void platformUpdate()
	{
#if _WIN32
		if (Windows::Dwm::isDarkMode() != Windows::Dwm::isDarkModeApplied())
		{
			Windows::Dwm::setThemeBasedOnUserMode((HWND)Renderer::platformWindow());
		}
#endif
	}

	static void platformEndRender()
	{
#if _WIN32
		Windows::Dwm::flush();
#endif
	}

	static inline void drawDebugWindow()
	{
#if EF_DEBUG
		ImGui::Begin("Debug");

		Ui::setXSpacingNoMin(Ui::xSpacingStr(Path::settingsDirectory));
		const auto dimension{ Renderer::windowDimension() };
		const auto refreshRate{ Renderer::refreshRate(0).value_or(1) };
		ImGui::Text("%.2f - %.5f - %dHz - %.2fms - %d %d",
			Renderer::time(),
			Renderer::deltaTime(),
			refreshRate,
			1000.f / refreshRate,
			dimension.x,
			dimension.y);

		ImGui::SameLine();
		static bool isDemoOpen{};
		if (Ui::button("Open Demo"))
		{
			isDemoOpen = !isDemoOpen;
		}

		DEBUG_PERFORMANCE_UPDATE;
		DEBUG_PERFORMANCE_DRAW;
		Ui::labelXSpacing("Pattern");
		Debug::Ui::patternTextBoxTTY();

		static bool isSandboxOpen{};
		Ui::labelXSpacing("Sandbox");
		if (Ui::buttonItemWidth("Open"))
		{
			isSandboxOpen = !isSandboxOpen;
		}

		if (std::filesystem::is_directory(Path::settingsDirectory))
		{
			Ui::labelXSpacing(Path::settingsDirectory);
			if (Ui::buttonItemWidth("Delete"))
			{
				std::filesystem::remove_all(Path::settingsDirectory);
			}
		}

		ImGui::End();

		if (isDemoOpen)
		{
			ImGui::ShowDemoWindow(&isDemoOpen);
		}
		if (isSandboxOpen)
		{
			Debug::Sandbox::drawWindow(&isSandboxOpen);
		}
#endif
	}

	void mainLoop()
	{
		System::init();

		bool firstUse{ true };

		static constexpr std::array<const char*, 4> gameWindowsName
		{
			"Freecam", "Controls", "Settings", "Bonus"
		};

		Dockspace dockspace{ !std::filesystem::exists(Path::imguiIni()) };
		dockspace.dock("State", ImGuiDir_Up, 0.175f);
		dockspace.dock(gameWindowsName, ImGuiDir_Left, 0.406f);
		dockspace.dock("Console", ImGuiDir_Down, 0.53f);
		dockspace.dock(_System, ImGuiDir_Right, 0.f);

		while (!Renderer::shouldClose())
		{
			Renderer::beginRender();
			SDL::Event::update();

			DEBUG_PERFORMANCE_BEGIN(_System);
			System::platformUpdate();
			Keyboard::update();
			GamepadWrapper::update();
			State::update();
			Welcome::drawModal(&g_settings.isWelcomeModalOpen);
			dockspace.drawWindow();
			State::drawWindow();
			Console::drawWindow();
			g_settings.drawWindow();
			DEBUG_PERFORMANCE_END(_System);

			auto* const game{ State::game() };
			if (game)
			{
				DEBUG_PERFORMANCE_BEGIN(_Game);
				if (game->isValid())
				{
					game->update();
				}
				game->draw();
				DEBUG_PERFORMANCE_END(_Game);
			}
			else
			{
				DRAW_GAME_WINDOWS(0, 0, 0, 0);
			}

			if (firstUse)
			{
				ImGui::SetWindowFocus("Freecam");
				firstUse = false;
			}

			System::drawDebugWindow();
			Renderer::endRender(g_settings.rgbFont(), g_settings.multiViewportsAlwaysOnTop);
			System::platformEndRender();
		}

		System::destroy();
	}
}
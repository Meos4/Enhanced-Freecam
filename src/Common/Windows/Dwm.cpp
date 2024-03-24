#include "Dwm.hpp"

#include <dwmapi.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace Windows::Dwm
{
	static BOOL darkModeApplied{ FALSE };

	bool isEnabled()
	{
		BOOL enabled{ FALSE };
		DwmIsCompositionEnabled(&enabled);
		return static_cast<bool>(enabled);
	}

	bool isDarkModeApplied()
	{
		return static_cast<bool>(darkModeApplied);
	}

	bool isDarkMode()
	{
		const auto uiSettings{ winrt::Windows::UI::ViewManagement::UISettings() };
		const auto foregroundColors{ uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Foreground) };
		return (((5 * foregroundColors.G) + (2 * foregroundColors.R) + foregroundColors.B) > (8 * 128));
	}

	void setThemeBasedOnUserMode(HWND hwnd)
	{
		if (!Windows::Dwm::isEnabled())
		{
			return;
		}

		darkModeApplied = static_cast<BOOL>(Windows::Dwm::isDarkMode());
		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkModeApplied, sizeof(darkModeApplied));
	}

	void flush()
	{
		DwmFlush();
	}
}
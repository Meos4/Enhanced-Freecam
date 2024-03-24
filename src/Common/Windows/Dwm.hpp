#pragma once

#include "Header.hpp"

namespace Windows::Dwm
{
	bool isEnabled();
	bool isDarkModeApplied();
	bool isDarkMode();
	void setThemeBasedOnUserMode(HWND hwnd);
	void flush();
}
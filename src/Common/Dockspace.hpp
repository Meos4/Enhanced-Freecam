#pragma once

#include "imgui.h"

#include <span>
#include <vector>

class Dockspace final
{
public:
	Dockspace(bool shouldDock);

	void drawWindow();
	void dock(const char* windowName, ImGuiDir dir, float ratio);
	void dock(std::span<const char* const> windowNames, ImGuiDir dir, float ratio);
private:
	struct Assignement
	{
		std::vector<const char*> names;
		ImGuiDir dir;
		float ratio;
	};

	std::vector<Dockspace::Assignement> m_assignement;
	bool m_shouldDock;
};
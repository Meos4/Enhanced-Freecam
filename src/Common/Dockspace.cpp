#include "Dockspace.hpp"

#include "imgui_internal.h"

Dockspace::Dockspace(bool shouldDock)
	: m_shouldDock(shouldDock)
{
}

void Dockspace::drawWindow()
{
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		static constexpr auto flags
		{
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus
		};

		const auto& viewport{ *ImGui::GetMainViewport() };
		ImGui::SetNextWindowPos(viewport.Pos);
		ImGui::SetNextWindowSize(viewport.Size);
		ImGui::SetNextWindowViewport(viewport.ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.f, 0.f });

		ImGui::Begin("Dockspace", nullptr, flags);

		ImGui::PopStyleVar(3);
		auto dockspaceId{ ImGui::GetID("Dockspace") };
		ImGui::DockSpace(dockspaceId, { 0.f, 0.f }, ImGuiDockNodeFlags_PassthruCentralNode);

		if (m_shouldDock)
		{
			ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderGetNode(dockspaceId)->LocalFlags &= ~ImGuiDockNodeFlags_CentralNode;
			ImGui::DockBuilderSetNodeSize(dockspaceId, viewport.Size);

			for (const auto& [names, dir, ratio] : m_assignement)
			{
				const auto nodeId{ ImGui::DockBuilderSplitNode(dockspaceId, dir, ratio, nullptr, &dockspaceId) };
				for (auto name : names)
				{
					ImGui::DockBuilderDockWindow(name, nodeId);
				}
			}

			ImGui::DockBuilderFinish(dockspaceId);
			m_shouldDock = false;
		}

		ImGui::End();
	}
}

void Dockspace::dock(const char* windowName, ImGuiDir dir, float ratio)
{
	m_assignement.emplace_back(Dockspace::Assignement{{ windowName }, dir, ratio });
}

void Dockspace::dock(std::span<const char* const> windowNames, ImGuiDir dir, float ratio)
{
	Dockspace::Assignement assignement;

	for (auto name : windowNames)
	{
		assignement.names.emplace_back(name);
	}

	assignement.dir = dir;
	assignement.ratio = ratio;
	m_assignement.emplace_back(assignement);
}
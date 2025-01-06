#include "Welcome.hpp"

#include "Settings.hpp"
#include "Software.hpp"
#include "Ui.hpp"

namespace Welcome
{
	void drawModal(bool* isOpen)
	{
		if (*isOpen)
		{
			ImGui::OpenPopup("Welcome");
			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, { 0.5f, 0.5f });

			if (ImGui::BeginPopupModal("Welcome", isOpen, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("%s v%s", Software::name, Software::version);

				Ui::separatorText("PlayStation 1");
				ImGui::TextUnformatted("Emulator:");
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, Ui::color(Ui::Color::DuckStation));
				ImGui::TextUnformatted("DuckStation");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::TextUnformatted("(up to date)");
				ImGui::TextUnformatted("Requirements: Settings -> Console -> CPU Emulation | Execution Mode: Interpreter (Slowest)");

				Ui::separatorText("PlayStation 2");
				ImGui::TextUnformatted("Emulator:");
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, Ui::color(Ui::Color::PCSX2));
				ImGui::TextUnformatted("PCSX2");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::TextUnformatted("(up to date)");
				ImGui::Text("Requirements 1: Download the pnach file when %s asks for it", Software::name);
				ImGui::TextUnformatted("Requirements 2: Settings -> Emulation -> System Settings | Enable Cheats. Then restart the game");
				
				Ui::setXSpacingStr("PCSX2 Cheats Path");
				g_settings.drawPCSX2CheatsPathInputText();

				const auto buttonSize{ 120.f };
				const auto shift{ (ImGui::GetContentRegionAvail().x - (buttonSize + ImGui::GetStyle().FramePadding.x * 2.0f)) * 0.5f };
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + shift);

				if (Ui::button("OK", { buttonSize, 0.f }))
				{
					*isOpen = false;
				}

				ImGui::EndPopup();
			}
		}
	}
}
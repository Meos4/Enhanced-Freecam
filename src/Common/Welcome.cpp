#include "Welcome.hpp"

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
				ImGui::PushStyleColor(ImGuiCol_Text, 0xFF'00'67'FF);
				ImGui::TextUnformatted("DuckStation");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::TextUnformatted("(up to date)");
				ImGui::TextUnformatted("Requirements: Settings -> Console -> CPU Emulation | Execution Mode: Interpreter (Slowest)");

				Ui::separatorText("PlayStation 2");
				ImGui::TextUnformatted("Emulator:");
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, 0xFF'FA'C3'38);
				ImGui::TextUnformatted("PCSX2");
				ImGui::PopStyleColor();
				ImGui::SameLine();
				ImGui::TextUnformatted("(any version)");
				ImGui::TextUnformatted("Requirements <= 1.6.0: System -> Enable Cheats");
				ImGui::TextUnformatted("Requirements >= 1.7.0: Settings -> Emulation -> System Settings | Enable Cheats. Then restart the game");
				ImGui::Text("Requirements any version: Download the pnach file when %s asks for it", Software::name);
				ImGui::Text("(The \"cheats\" path should be in the PCSX2 directory but it is possible to modify it\nin PCSX2 or in the System \"Advanced\" settings of %s)", Software::name);

				Ui::separatorText("Gamepad");
				ImGui::TextUnformatted("Requires an Xbox gamepad (XInput), if you want to use another one, you need an external utility (Ex: DS4Windows) to emulate it");

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
#include "Debug.hpp"

#if EF_DEBUG
namespace PS2::Debug
{
	void drawPCSX2CheatsPnachWindow(const std::filesystem::path& cheatsPath, bool* isOpen)
	{
		ImGui::Begin("PCSX2 Cheats", isOpen);

		try
		{
			s32 id{};
			for (const auto& entry : std::filesystem::directory_iterator(cheatsPath))
			{
				ImGui::PushID(id++);
				if (Ui::button("X"))
				{
					std::filesystem::remove_all(entry.path());
				}
				ImGui::PopID();

				ImGui::SameLine();
				ImGui::Text("%s", entry.path().string().c_str());
			}
		}
		catch ([[maybe_unused]] const std::filesystem::filesystem_error& e)
		{
		}

		ImGui::End();
	}
}
#endif
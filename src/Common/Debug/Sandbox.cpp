#include "Sandbox.hpp"

#if EF_DEBUG
#include "Common/Debug/Ui.hpp"
#include "Common/PS1/PS1.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/Process.hpp"
#include "Common/Ram.hpp"
#include "Common/Types.hpp"
#include "Common/Ui.hpp"

#include <array>
#include <limits>
#include <memory>

namespace Debug::Sandbox
{
	enum
	{
		MODE_UNKNOWN,
		MODE_PS1,
		MODE_PS2,
		MODE_COUNT
	};

	static constexpr std::array<const char*, MODE_COUNT> modeNames
	{
		"Unknown",
		"PS1",
		"PS2"
	};

	static std::shared_ptr<Process> process;
	static std::uintptr_t ramBegin{};
	static std::unique_ptr<Ram> ram;
	static s32 mode{ MODE_UNKNOWN };

	static void update()
	{

	}

	void drawWindow(bool* isOpen)
	{
		ImGui::Begin("Sandbox", isOpen);

		const auto modeButtonSize
		{ 
			(ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * (MODE_COUNT - 1)) / static_cast<float>(MODE_COUNT) 
		};
		for (std::size_t i{}; i < MODE_COUNT; ++i)
		{	
			const bool colored{ mode == i };
			if (colored)
			{
				::Ui::pushStyleColor(::Ui::color(::Ui::Color::GreenY), ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActive);
			}
			if (::Ui::button(modeNames[i], { modeButtonSize, 0 }))
			{
				mode = i;
				ram.reset();
			}
			if (colored)
			{
				ImGui::PopStyleColor(3);
			}

			if (i != MODE_COUNT - 1)
			{
				ImGui::SameLine();
			}
		}

		auto processInfoFormatted = [](const Process& process)
		{
			return std::format("{} ({}) ({} bits)",
				process.name(),
				process.pId(),
				process.architecture() == Process::Architecture::x86 ? "32" : "64");
		};

		if (::Ui::button("X##Sandbox Process Reset", ::Ui::buttonOneLetterSize()))
		{
			process.reset();
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		if (ImGui::BeginCombo("##Sandbox Process Names", process ? processInfoFormatted(*process).c_str() : "", ImGuiComboFlags_None))
		{
			for (auto& p : Process::taskBarProcessList())
			{
				if (ImGui::Selectable(processInfoFormatted(*p).c_str()) && !(process && process->pId() == p->pId()))
				{
					process = std::move(p);
					break;
				}
			}

			ImGui::EndCombo();
		}

		if (!ram)
		{
			ImGui::BeginDisabled(process ? false : true);
			if (::Ui::button("Create Ram"))
			{
				switch (mode)
				{
				case MODE_UNKNOWN:
				{
					const auto ramSize
					{ 
						process->architecture() == Process::Architecture::x86 ? 
							std::numeric_limits<u32>::max() : std::numeric_limits<u64>::max() 
					};
					ram = std::make_unique<Ram>(process, ramBegin, ramSize, true);
					break;
				}
				case MODE_PS1:
				{
					ram = std::make_unique<Ram>(PS1::createRam(process, ramBegin));
					break;
				}
				case MODE_PS2:
				{
					ram = std::make_unique<Ram>(PS2::createRam(process, ramBegin));
					break;
				}
				}
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			::Ui::drag("##Sandbox Ram Begin", &ramBegin, 4.f, "%X", ImGuiSliderFlags_AlwaysClamp);
		}
		else
		{
			if (::Ui::button("X##Sandbox Ram Reset", ::Ui::buttonOneLetterSize()))
			{
				ram.reset();
			}
			else
			{
				ImGui::SameLine();
				ImGui::Text("Ram -> %s (0x%X)", processInfoFormatted(ram->process()).c_str(), ram->begin());
			}
		}

		if (ram)
		{
			auto isOpenWindowButton = [](const char* label, bool* isOpen)
			{
				if (::Ui::button(label, { ImGui::GetContentRegionAvail().x, 0.f }))
				{
					*isOpen = !*isOpen;
				}
				return *isOpen;
			};

			::Ui::separatorText(modeNames[mode]);
			if (mode == MODE_PS1 || mode == MODE_PS2)
			{
				static bool isMipsCallOpen{};
				if (isOpenWindowButton("Mips Call##Sandbox", &isMipsCallOpen))
				{
					Debug::Ui::mipsCallWindow(*ram, &isMipsCallOpen);
				}
			}

			::Ui::separatorText("Update");
			Sandbox::update();
		}

		ImGui::End();
	}
}
#endif
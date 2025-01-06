#include "Sandbox.hpp"

#if EF_DEBUG
#include "Common/Debug/Ui.hpp"
#include "Common/PS2/PCSX2.hpp"
#include "Common/PS1/PS1.hpp"
#include "Common/PS2/PS2.hpp"

#include "Common/Console.hpp"
#include "Common/Process.hpp"
#include "Common/Ram.hpp"
#include "Common/Settings.hpp"
#include "Common/Types.hpp"
#include "Common/Ui.hpp"
#include "Common/Util.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
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
	static struct
	{
		std::array<char, 9> crc;
		u32 textSectionBegin;
		u32 textSectionEnd;
	} pcsx2{};

	static void update()
	{

	}

	static void setRamBeginFromProcess()
	{
		if (process)
		{
			if (mode == MODE_PS1)
			{
				// NO$PSX 2.0
				if (Util::isProcessName(*process, "no$psx"))
				{
					u32 ps1Begin{};
					process->read(0x00491C80, &ps1Begin, sizeof(ps1Begin));
					if (ps1Begin)
					{
						ramBegin = ps1Begin;
					}
				}
			}
			else if (mode == MODE_PS2)
			{
				if (process->architecture() == Process::Architecture::x86 && Util::isProcessName(*process, "pcsx2"))
				{
					ramBegin = 0x20000000;
				}
			}
		}
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
					Sandbox::setRamBeginFromProcess();
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
					ram = std::make_unique<Ram>(process, std::make_unique<RamRWEmulator>(process.get(), ramBegin, true), ramSize);
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

			if (mode == MODE_PS2 && Util::isProcessName(ram->process(), "pcsx2"))
			{
				const bool crcContainsNull{ std::any_of(pcsx2.crc.begin(), pcsx2.crc.end() - 1, [](auto ch){ return ch == '\0'; }) };
				ImGui::BeginDisabled(pcsx2.textSectionBegin >= pcsx2.textSectionEnd || crcContainsNull);
				if (::Ui::button("Create Pnach##Sandbox"))
				{
					std::filesystem::path path{ g_settings.pcsx2.cheatsPath };

					if (std::filesystem::is_directory(path))
					{
						path = std::format("{}/{} force jit.pnach", path.string().c_str(), pcsx2.crc.data());
						std::ofstream pnach{ path };
						const auto iteration{ ((pcsx2.textSectionEnd & ~0xFFF) / 0x1000) - ((pcsx2.textSectionBegin & ~0xFFF) / 0x1000) + 1 };

						for (u32 i{}; i < iteration; ++i)
						{
							const auto offset{ pcsx2.textSectionBegin + (i * 0x1000) };
							pnach << "patch=1,EE," << std::hex << offset << ",extended," << std::uppercase << +ram->read<u8>(offset) << '\n';
						}

						Console::append(Console::Type::Success, "{} created", path.string().c_str());
					}
					else
					{
						Console::append(Console::Type::Error, "{} doesn't exist", path.string().c_str());
					}
				}
				ImGui::EndDisabled();

				ImGui::SameLine();
				const auto widgetSize{ (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2.f) / 3.f };
				ImGui::SetNextItemWidth(widgetSize);
				ImGui::InputText("##Sandbox Crc", pcsx2.crc.data(), pcsx2.crc.size() , ImGuiInputTextFlags_CharsUppercase);

				auto dragTextSection = [](const char* label, u32* val)
				{
					::Ui::drag(label, val, 4.f, "%X", ImGuiSliderFlags_AlwaysClamp, u32(0), PS2::memSize);
				};

				ImGui::SameLine();
				ImGui::SetNextItemWidth(widgetSize);
				dragTextSection("##Sandbox Text Section Begin", &pcsx2.textSectionBegin);
				
				ImGui::SameLine();
				ImGui::SetNextItemWidth(widgetSize);
				dragTextSection("##Sandbox Text Section End", &pcsx2.textSectionEnd);
			}

			::Ui::separatorText("Update");
			Sandbox::update();
		}

		ImGui::End();
	}
}
#endif
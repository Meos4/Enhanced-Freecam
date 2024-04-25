#pragma once

#if EF_DEBUG
#include "Common/Debug/Ui.hpp"

#include "Common/Ram.hpp"
#include "Common/Types.hpp"
#include "Common/Ui.hpp"

#include "PS1.hpp"

#include <format>

#define PS1_DEBUG_DRAW_WINDOW PS1::Debug::drawWindow(&m_game)

namespace PS1::Debug
{
	template <typename T>
	void drawWindow(T* game)
	{
		ImGui::Begin(PS1::settingsName);

		Ui::setXSpacingNoMin(Ui::xSpacingStr("Write Analyzer"));

		const auto& ram{ game->ram() };
		const auto ptrFormat{ ram.process().architecture() == Process::Architecture::x86 ? "{:08X}" : "{:016X}" };
		const auto ptrFormatted{ std::vformat(ptrFormat, std::make_format_args(ram.begin())) };

		Ui::labelXSpacing("Begin");
		ImGui::Text("%s", ptrFormatted.c_str());

		auto isOpenWindowButton = [](const char* label, bool* isOpen)
		{
			Ui::labelXSpacing(label);
			if (Ui::buttonItemWidth(std::format("Open##{}", label).c_str()))
			{
				*isOpen = !*isOpen;
			}
			return *isOpen;
		};

		static bool isMipsCallOpen{};
		if (isOpenWindowButton("Mips Call", &isMipsCallOpen))
		{
			::Debug::Ui::mipsCallWindow(ram, &isMipsCallOpen);
		}

		static bool isOffsetOpen{};
		if (isOpenWindowButton("Offset", &isOffsetOpen))
		{
			const auto& offsets{ game->offset() };
			::Debug::Ui::offsetWindow((u32*)&offsets, sizeof(offsets) / sizeof(u32), ram, &isOffsetOpen);
		}

		static bool isWAOpen{};
		if (isOpenWindowButton("Write Analyzer", &isWAOpen))
		{
			::Debug::WriteAnalyzer::drawWindow(&isWAOpen);
		}

		ImGui::End();
	}
}
#else
#define PS1_DEBUG_DRAW_WINDOW
#endif
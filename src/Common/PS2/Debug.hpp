#pragma once

#if EF_DEBUG
#include "Common/Debug/Ui.hpp"

#include "Common/Console.hpp"
#include "Common/Ram.hpp"
#include "Common/Settings.hpp"
#include "Common/Types.hpp"
#include "Common/Ui.hpp"
#include "Common/Util.hpp"

#include "PS2.hpp"

#include <filesystem>
#include <format>
#include <fstream>

#define PS2_DEBUG_DRAW_WINDOW PS2::Debug::drawWindow(&m_game)

namespace PS2::Debug
{
	void drawPCSX2CheatsPnachWindow(const std::filesystem::path& cheatsPath, bool* isOpen);

	template <typename T>
	void drawWindow(T* game)
	{
		ImGui::Begin(PS2::settingsName);

		const auto& ram{ game->ram() };
		const bool isPcsx2{ Util::isProcessName(ram.process(), "pcsx2") };
		Ui::setXSpacingStr(isPcsx2 ? "Pnach Force Jit" : "Write Analyzer");

		const auto ramBegin{ ram.begin() };
		const auto ptrFormat{ ram.process().architecture() == Process::Architecture::x86 ? "{:08X}" : "{:016X}" };
		const auto ptrFormatted{ std::vformat(ptrFormat, std::make_format_args(ramBegin)) };

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

		if (isPcsx2)
		{
			Ui::separatorText("PCSX2");

			const auto& pnachInfo{ game->pnachInfo() };

			static bool isPCSX2CheatsOpen{};
			if (isOpenWindowButton("Cheats", &isPCSX2CheatsOpen))
			{
				std::filesystem::path path;

				if (g_settings.pcsx2.useDifferentCheatsPath)
				{
					path = g_settings.pcsx2.cheatsPath;
					path = std::format("{}/{} ({} - ({}) force jit).pnach", path.string().c_str(), pnachInfo.crc, game->name, game->version());
				}
				else
				{
					path = ram.process().path();
					path.remove_filename();
					path = std::format("{}/cheats", path.string().c_str());
				}

				PS2::Debug::drawPCSX2CheatsPnachWindow(path, &isPCSX2CheatsOpen);
			}

			Ui::labelXSpacing("Pnach Force Jit");

			if (Ui::buttonItemWidth("Create"))
			{
				std::filesystem::path path;

				if (g_settings.pcsx2.useDifferentCheatsPath)
				{
					path = g_settings.pcsx2.cheatsPath;
				}
				else
				{
					path = ram.process().path();
					path.remove_filename();
					path = std::format("{}/cheats", path.string().c_str() );
				}

				if (std::filesystem::is_directory(path))
				{
					path = std::format("{}/{} ({} - ({}) force jit).pnach", path.string().c_str(), pnachInfo.crc, game->name, game->version());
					std::ofstream pnach{ path };
					const auto iteration{ ((pnachInfo.textSectionEnd & ~0xFFF) / 0x1000) - ((pnachInfo.textSectionBegin & ~0xFFF) / 0x1000) + 1 };

					for (u32 i{}; i < iteration; ++i)
					{
						const auto offset{ pnachInfo.textSectionBegin + (i * 0x1000) };
						pnach << "patch=1,EE," << std::hex << offset << ",extended," << std::uppercase << +ram.read<u8>(offset) << '\n';
					}

					Console::append(Console::Type::Success, "{} created", path.string().c_str());
				}
				else
				{
					Console::append(Console::Type::Error, "{} doesn't exist", path.string().c_str());
				}
			}

			Ui::labelXSpacing("Offset Fn 4096");
			const auto& offsets{ game->offset() };
			u32 nb4096{};

			for (std::size_t i{}; i < sizeof(offsets) / sizeof(u32); ++i)
			{
				const auto offset{ *((u32*)&offsets + i) };
				if (offset && offset % 4096 == 0 && offset <= pnachInfo.textSectionEnd)
				{
					++nb4096;
				}
			}

			ImGui::Text("%d", nb4096);
		}

		ImGui::End();
	}
}
#else
#define PS2_DEBUG_DRAW_WINDOW
#endif
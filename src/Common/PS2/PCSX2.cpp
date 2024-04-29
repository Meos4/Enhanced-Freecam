#include "PCSX2.hpp"

#include "Common/AsyncGameSearcher.hpp"
#include "Common/Buffer.hpp"
#include "Common/Console.hpp"
#include "Common/Renderer.hpp"
#include "Common/Settings.hpp"
#include "Common/Software.hpp"
#include "Common/Ui.hpp"
#include "Common/Util.hpp"

#include "PS2.hpp"

#include <format>
#include <fstream>
#include <thread>

namespace PS2::PCSX2
{
	static bool createPnachForceJit(const std::filesystem::path& path, const Ram& ram,
		const std::set<u32>& offsets, const char* game, const char* version)
	{
		std::ofstream pnach{ path };

		if (!pnach.is_open())
		{
			return false;
		}

		pnach << "gametitle=" << game << " " << version;
		pnach << "\ncomment=" << Software::name << " " << Software::version << '\n';
		for (auto offset : offsets)
		{
			pnach << "\npatch=1,EE," << std::hex << offset << ",extended," << std::uppercase << +ram.read<u8>(offset);
		}

		return true;
	}

	static bool isPnachSameSoftwareVersion(const std::filesystem::path& path)
	{
		std::ifstream pnach{ path };

		if (!pnach.is_open())
		{
			return false;
		}

		std::string line;
		std::getline(pnach, line);
		if (std::getline(pnach, line) && line.find("comment=") != std::string::npos)
		{
			static constexpr auto
				versionPos{ Util::cstrSize("comment=") + Util::cstrSize(Software::name) + Util::cstrSize(" ") },
				expectedLineSize{ versionPos + Util::cstrSize(Software::version) };

			return line.size() >= expectedLineSize && line.substr(versionPos) == Software::version;
		}

		return false;
	}

	std::set<u32> textSectionOffsets(u32* offset, std::size_t size, u32 textSectionBegin, u32 textSectionEnd)
	{
		std::set<u32> textOffsets;

		for (std::size_t i{}; i < size; ++i)
		{
			if (offset[i] < textSectionEnd)
			{
				auto isValid = [&](u32 off)
				{
					return off <= textSectionEnd && off >= textSectionBegin;
				};

				const auto offset4096{ offset[i] & ~0xFFF };
				if (isValid(offset4096))
				{
					textOffsets.insert(offset4096);
				}
				if (isValid(offset4096 + 0x1000))
				{
					textOffsets.insert(offset4096 + 0x1000);
				}
			}
		}

		return textOffsets;
	}

	void drawCreatePnachTextSectionWindow(const Ram& ram, const std::set<u32>& offsets, const char* crc, const char* game, const char* version)
	{
		static constexpr auto wndName{ "Pnach" };
		std::filesystem::path path;

		if (g_settings.pcsx2.useDifferentCheatsPath)
		{
			path = g_settings.pcsx2.cheatsPath;
		}
		else
		{
			path = ram.process().path();
			path.remove_filename();
			path = std::format("{}cheats", path.string().c_str());
		}

		if (!std::filesystem::is_directory(path))
		{
			ImGui::SetNextWindowSize({ 512.f, 128.f }, ImGuiCond_FirstUseEver);
			ImGui::Begin(wndName);

			u32 color;
			if (static_cast<u64>(Renderer::time()) % 2)
			{
				color = 0xFF'00'00'FF;
			}
			else
			{
				color = 0xFF'00'00'C0;
			}

			ImGui::PushStyleColor(ImGuiCol_Text, color);
			ImGui::TextUnformatted("Can't find PCSX2 cheats path, you need to set it in System -> Advanced");
			if (!g_settings.pcsx2.useDifferentCheatsPath)
			{
				ImGui::TextUnformatted("or create a cheats directory inside PCSX2 directory and set the path in");
				if (ram.process().architecture() == Process::Architecture::x86)
				{
					ImGui::TextUnformatted("PCSX2 -> Config -> Plugin/BIOS Selector -> Folders -> Cheats:");
				}
				else
				{
					ImGui::TextUnformatted("PCSX2 -> Settings -> Folders -> Cheats Directory");
				}
			}
			ImGui::PopStyleColor();
			ImGui::End();

			return;
		}

		path = std::format("{}\\{} ({} - ({})).pnach", path.string().c_str(), crc, game, version);

		auto drawDownloadWindow = [&](const char* label, const char* success, const char* error)
		{
			if (!Ui::downloadFileConfirmationWindow(wndName, label, path))
			{
				return;
			}

			if (PCSX2::createPnachForceJit(path, ram, offsets, game, version))
			{
				Console::append(Console::Type::Success, success);
			}
			else
			{
				Console::append(Console::Type::Error, error);
			}
		};

		if (std::filesystem::is_regular_file(path))
		{
			const auto rendererTime{ Renderer::time() };

			if (rendererTime < g_settings.pcsx2.pnachVerificationTime)
			{
				return;
			}

			if (PCSX2::isPnachSameSoftwareVersion(path))
			{
				g_settings.pcsx2.pnachVerificationTime = rendererTime + g_settings.pcsx2.pnachVerificationDelay;
				return;
			}

			const auto label{ std::format("{} need to update the existing pnach file", Software::name) };
			drawDownloadWindow(label.c_str(), "Pnach updated successfully", "Unable to update pnach");
		}
		else
		{
			const auto label{ std::format("{} need to create the required pnach file for its proper use", Software::name) };
			static constexpr auto success{ "Pnach created successfully, be sure to enable cheats in PCSX2 and restart your game" };
			drawDownloadWindow(label.c_str(), success, "Unable to create pnach");
		}
	}

	std::optional<std::uintptr_t> eememPtr(const Process& process)
	{
		const auto eemem{ process.procAddress("EEmem") };

		if (!eemem.has_value())
		{
			return std::nullopt;
		}

		return eemem.value();
	}

	std::optional<std::uintptr_t> eememVal(const Process& process)
	{
		const auto eememPtr{ PCSX2::eememPtr(process) };

		if (!eememPtr.has_value())
		{
			return std::nullopt;
		}

		if (process.architecture() == Process::Architecture::x86)
		{
			u32 begin{};
			process.read(eememPtr.value(), &begin, sizeof(begin));
			return static_cast<std::uintptr_t>(begin);
		}
		else
		{
			u64 begin{};
			process.read(eememPtr.value(), &begin, sizeof(begin));
			return static_cast<std::uintptr_t>(begin);
		}
	}

	std::uintptr_t eememAGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
	{
		// from PCSX2
		// https://github.com/PCSX2/pcsx2/blob/master/pcsx2/vtlb.h
		static constexpr u32 VTLB_PAGE_MASK = 4095;
		static constexpr u32 VTLB_PAGE_SIZE = 4096;
		
		const auto offsetMask{ op.offset & VTLB_PAGE_MASK };
		const auto bufferSize{ op.pattern.size() };
		Buffer buffer(bufferSize);
		auto* const bufferPtr{ buffer.data() };

		while (running)
		{
			for (const auto& [begin, size] : process.virtualMemoryPages())
			{
				if (size == VTLB_PAGE_SIZE)
				{
					process.read(begin + offsetMask, bufferPtr, bufferSize);
					if (Util::findBufferPatternIterator(buffer, op.pattern) != buffer.end())
					{
						return begin + offsetMask - op.offset;
					}
				}
			}
			std::this_thread::sleep_for(g_settings.searchGameThreadSleepDelay);
		}

		return AsyncGameSearcher::exitValue;
	}

	std::uintptr_t x86AGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
	{
		const auto bufferSize{ op.pattern.size() };
		Buffer buffer(bufferSize);
		auto* const bufferPtr{ buffer.data() };

		while (running)
		{
			static constexpr u32 begin{ 0x20000000 };
			process.read(begin + op.offset, bufferPtr, bufferSize);
			if (Util::findBufferPatternIterator(buffer, op.pattern) != buffer.end())
			{
				return begin;
			}
			std::this_thread::sleep_for(g_settings.searchGameThreadSleepDelay);
		}

		return AsyncGameSearcher::exitValue;
	}
}
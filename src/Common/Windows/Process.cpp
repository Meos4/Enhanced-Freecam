#include "Process.hpp"

#include <cstring>
#include <utility>
#include <TlHelp32.h>

namespace Windows
{
	Process::Process(std::filesystem::path&& path, DWORD pId, HANDLE pHandle, Process::Architecture architecture)
		: m_path(path), m_pId(pId), m_pHandle(pHandle), m_architecture(architecture)
	{
	}

	Process::~Process()
	{
		CloseHandle(m_pHandle);
	}

	std::unique_ptr<Process> Process::create(DWORD pId)
	{
		auto pHandle{ OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId) };
		const auto architecture{ Process::architecture(pHandle) };
		auto path{ Process::path(pHandle) };

		if (!architecture.has_value() || path.empty())
		{
			if (pHandle)
			{
				CloseHandle(pHandle);
			}
			return nullptr;
		}

#if !_WIN64
		if (architecture.value() == Process::Architecture::x64)
		{
			CloseHandle(pHandle);
			return nullptr;
		}
#endif

		return std::unique_ptr<Process>{ new Process{ std::move(path), pId, pHandle, architecture.value() } };
	}

	std::vector<std::unique_ptr<Process>> Process::taskBarProcessList()
	{
		std::vector<std::unique_ptr<Process>> processList;

		for (auto pId : Process::taskBarPId())
		{
			auto process{ Process::create(pId) };
			if (process)
			{
				processList.emplace_back(std::move(process));
			}
		}

		return processList;
	}

	std::filesystem::path Process::path(HANDLE pHandle)
	{
		std::filesystem::path path;

		if (pHandle)
		{
			char buffer[MAX_PATH]{};
			DWORD bufferSize{ MAX_PATH };

			if (QueryFullProcessImageName(pHandle, 0, buffer, &bufferSize))
			{
				path = buffer;
			}
		}

		return path;
	}

	DWORD Process::softwarePId()
	{
		return GetCurrentProcessId();
	}

	std::set<DWORD> Process::taskBarPId()
	{
		auto EnumWindowsCb = [](HWND hwnd, LPARAM lParam) -> BOOL
		{
			DWORD pId{};
			GetWindowThreadProcessId(hwnd, &pId);

			if (IsWindowVisible(hwnd) && pId)
			{
				// Try if the process allows to be opened
				auto pHandle{ OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId) };
				if (pHandle)
				{
					auto* const pIdList{ (std::set<DWORD>*)lParam };
					pIdList->insert(pId);
					CloseHandle(pHandle);
				}
			}

			return TRUE;
		};

		std::set<DWORD> pIdList;
		EnumWindows(EnumWindowsCb, (LPARAM)&pIdList);
		pIdList.erase(Process::softwarePId());

		return pIdList;
	}

	std::optional<Process::Architecture> Process::architecture(HANDLE pHandle)
	{
		BOOL isX86Process;
		if (IsWow64Process(pHandle, &isX86Process))
		{
			return isX86Process ? Process::Architecture::x86 : Process::Architecture::x64;
		}
		return std::nullopt;
	}

	bool Process::isValid() const
	{
		const auto process{ Process::create(m_pId) };
		if (process)
		{
			return process->path() == m_path;
		}
		return false;
	}

	std::string Process::name() const
	{
		return m_path.stem().string();
	}

	std::vector<Process::VirtualMemoryPage> Process::virtualMemoryPages() const
	{
		void* offset{ nullptr };
		MEMORY_BASIC_INFORMATION mbi;
		std::vector<Process::VirtualMemoryPage> vmp;

		while (VirtualQueryEx(m_pHandle, offset, &mbi, sizeof(mbi)))
		{
			if (mbi.State == MEM_COMMIT && (mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD)) == 0)
			{
				vmp.emplace_back(reinterpret_cast<std::uintptr_t>(mbi.BaseAddress), mbi.RegionSize);
			}

			offset = (u8*)offset + mbi.RegionSize;
		}

		return vmp;
	}

	std::optional<std::uintptr_t> Process::beginOffset() const
	{
		auto pSnap{ CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_pId) };
		if (pSnap == INVALID_HANDLE_VALUE)
		{
			return std::nullopt;
		}

		MODULEENTRY32 me;
		me.dwSize = sizeof(me);
		if (Module32First(pSnap, &me))
		{
			return reinterpret_cast<std::uintptr_t>(me.hModule);
		}

		return std::nullopt;
	}

	std::optional<std::uintptr_t> Process::procAddress(const char* name) const
	{
		const auto begin{ beginOffset().value_or(0) };
		if (!begin)
		{
			return std::nullopt;
		}

		IMAGE_DOS_HEADER dosHeader{};
		read(begin, &dosHeader, sizeof(dosHeader));
		if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
		{
			return std::nullopt;
		}

		std::uintptr_t exportOffset{};
		if (m_architecture == Process::Architecture::x86)
		{
			IMAGE_NT_HEADERS32 ntHeader{};
			read(begin + dosHeader.e_lfanew, &ntHeader, sizeof(ntHeader));

			if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
			{
				return std::nullopt;
			}

			exportOffset = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		}
		else
		{
			IMAGE_NT_HEADERS64 ntHeader{};
			read(begin + dosHeader.e_lfanew, &ntHeader, sizeof(ntHeader));

			if (ntHeader.Signature != IMAGE_NT_SIGNATURE)
			{
				return std::nullopt;
			}

			exportOffset = ntHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
		}

		if (!exportOffset)
		{
			return std::nullopt;
		}

		IMAGE_EXPORT_DIRECTORY exportDir{};
		read(begin + exportOffset, &exportDir, sizeof(exportDir));

		const std::uintptr_t namesOffset{ begin + exportDir.AddressOfNames };
		const auto strSize{ std::strlen(name) };
		std::vector<char> buffer(strSize);
		auto* const bufferPtr{ buffer.data() };

		for (DWORD i{}; i < exportDir.NumberOfNames; ++i)
		{
			DWORD nameOffset{};
			read(namesOffset + i * sizeof(nameOffset), &nameOffset, sizeof(nameOffset));
			read(begin + nameOffset, bufferPtr, strSize);

			if (std::memcmp(name, bufferPtr, strSize) == 0)
			{
				WORD ordinal{};
				read(begin + exportDir.AddressOfNameOrdinals + i * sizeof(ordinal), &ordinal, sizeof(ordinal));
				DWORD targetOffset{};
				read(begin + exportDir.AddressOfFunctions + ordinal * sizeof(targetOffset), &targetOffset, sizeof(targetOffset));
				return begin + targetOffset;
			}
		}

		return std::nullopt;
	}

	void Process::read(std::uintptr_t offset, void* val, std::size_t size) const
	{
		ReadProcessMemory(m_pHandle, (void*)offset, val, size, nullptr);
	}

	void Process::write(std::uintptr_t offset, void* val, std::size_t size) const
	{
		DWORD oldProtect{};
		VirtualProtectEx(m_pHandle, (void*)offset, size, PAGE_EXECUTE_READWRITE, &oldProtect);
		WriteProcessMemory(m_pHandle, (void*)offset, val, size, nullptr);
		VirtualProtectEx(m_pHandle, (void*)offset, size, oldProtect, &oldProtect);
	}

	void Process::writeNoProtect(std::uintptr_t offset, void* val, std::size_t size) const
	{
		WriteProcessMemory(m_pHandle, (void*)offset, val, size, nullptr);
	}

	const std::filesystem::path& Process::path() const
	{
		return m_path;
	}

	DWORD Process::pId() const
	{
		return m_pId;
	}

	Process::Architecture Process::architecture() const
	{
		return m_architecture;
	}
}
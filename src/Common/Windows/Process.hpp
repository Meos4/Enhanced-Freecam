#pragma once

#include "Common/Types.hpp"

#include "Header.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace Windows
{
	class Process final
	{
	public:
		enum class Architecture
		{
			x86, x64
		};

		struct VirtualMemoryPage
		{
			std::uintptr_t begin;
			std::size_t size;
		};

		~Process();

		static std::unique_ptr<Process> create(DWORD pId);
		static std::vector<std::unique_ptr<Process>> taskBarProcessList();
		static std::filesystem::path path(HANDLE pHandle);
		static DWORD softwarePId();
		static std::set<DWORD> taskBarPId();
		static std::optional<Process::Architecture> architecture(HANDLE pHandle);

		bool isValid() const;
		std::string name() const;
		std::vector<Process::VirtualMemoryPage> virtualMemoryPages() const;
		std::optional<std::uintptr_t> beginOffset() const;
		std::optional<std::uintptr_t> procAddress(const char* name) const;
		void read(std::uintptr_t offset, void* val, std::size_t size) const;
		void write(std::uintptr_t offset, void* val, std::size_t size) const;
		void writeNoProtect(std::uintptr_t offset, void* val, std::size_t size) const;

		const std::filesystem::path& path() const;
		DWORD pId() const;
		Process::Architecture architecture() const;
	private:
		Process(std::filesystem::path&& path, DWORD pId, HANDLE pHandle, Process::Architecture architecture);

		std::filesystem::path m_path;
		DWORD m_pId;
		HANDLE m_pHandle;
		Process::Architecture m_architecture;
	};
}
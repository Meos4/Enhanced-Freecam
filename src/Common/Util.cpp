#include "Util.hpp"

#include <cctype>
#include <ranges>

namespace Util
{
	void toLower(std::string* str)
	{
		std::transform(str->begin(), str->end(), str->begin(), ::tolower);
	}

	bool isValidProcessPatternOffset(const Process& process, std::uintptr_t offset, std::span<const u8> pattern)
	{
		const auto patternSize{ pattern.size() };
		Buffer buffer(patternSize);
		process.read(offset, buffer.data(), patternSize);
		return Util::findBufferPatternIterator(buffer, pattern) != buffer.end();
	}

	bool isProcessName(const Process& process, const char* name)
	{
		auto processName{ process.name() };
		Util::toLower(&processName);
		return processName.find(name) != std::string::npos;
	}

	std::vector<Process::VirtualMemoryPage> createContiguousVmp(const std::vector<Process::VirtualMemoryPage>& vmp, std::size_t pageSizeMin)
	{
		std::vector<Process::VirtualMemoryPage> cVmp;

		if (!vmp.empty())
		{
			std::uintptr_t cBegin{ vmp[0].begin };
			std::size_t cSize{ vmp[0].size };

			for (const auto& [begin, size] : std::ranges::drop_view{ vmp, 1 })
			{
				if (cBegin + cSize == begin)
				{
					cSize += size;
				}
				else
				{
					if (cSize >= pageSizeMin)
					{
						cVmp.emplace_back(cBegin, cSize);
					}

					cBegin = begin;
					cSize = size;
				}
			}
		}

		return cVmp;
	}
}
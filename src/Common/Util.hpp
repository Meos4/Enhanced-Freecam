#pragma once

#include "Buffer.hpp"
#include "Process.hpp"
#include "TemplateTypes.hpp"
#include "Types.hpp"

#include <algorithm>
#include <functional>
#include <limits>
#include <span>
#include <string>
#include <vector>

namespace Util
{
	template <typename... Ts>
	struct Overload : Ts...
	{
		using Ts::operator()...;
	};

	void toLower(std::string* str);
	bool isValidProcessPatternOffset(const Process& process, std::uintptr_t offset, std::span<const u8> pattern);
	bool isProcessName(const Process& process, const char* name);
	std::vector<Process::VirtualMemoryPage> createContiguousVmp(const std::vector<Process::VirtualMemoryPage>& vmp, std::size_t pageSizeMin);

	constexpr std::size_t cstrSize(const char* str)
	{
		std::size_t size{};
		while (*(str++))
		{
			++size;
		}
		return size;
	}

	[[nodiscard]] constexpr u8 hexCharToU8(const char* hexChar)
	{
		u8 val{};
		for (s32 i{}; i < 2; ++i)
		{
			if (hexChar[i] <= 57)
			{
				val += (hexChar[i] - 48) * (1 << (4 * (2 - 1 - i)));
			}
			else
			{
				val += (hexChar[i] - 55) * (1 << (4 * (2 - 1 - i)));
			}
		}
		return val;
	}

	constexpr Buffer cstrToBufferByte(const char* str)
	{
		const auto size{ (Util::cstrSize(str) + 1) / 3 };
		Buffer buffer(size);

		for (std::size_t i{}; i < size; ++i)
		{
			buffer[i] = Util::hexCharToU8(str + i * 3);
		}

		return buffer;
	}

	constexpr Buffer::const_iterator findBufferPatternIterator(const Buffer& buffer, std::span<const u8> pattern)
	{
		return std::search(buffer.begin(), buffer.end(), std::boyer_moore_searcher(pattern.begin(), pattern.end()));
	}
}
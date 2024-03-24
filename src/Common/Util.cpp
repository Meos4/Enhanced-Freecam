#include "Util.hpp"

#include <cctype>

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
}
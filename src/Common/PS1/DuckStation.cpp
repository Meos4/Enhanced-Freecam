#include "DuckStation.hpp"

#include "Common/AsyncGameSearcher.hpp"
#include "Common/Buffer.hpp"
#include "Common/Settings.hpp"
#include "Common/Util.hpp"

#include <cstring>
#include <thread>

namespace PS1::DuckStation
{
	std::optional<std::uintptr_t> ramPtr(const Process& process)
	{
		const auto ram{ process.procAddress("RAM") };

		if (!ram.has_value())
		{
			return std::nullopt;
		}

		return ram.value();
	}

	std::optional<std::uintptr_t> ramVal(const Process& process)
	{
		const auto ramPtr{ DuckStation::ramPtr(process) };

		if (!ramPtr.has_value())
		{
			return std::nullopt;
		}

		if (process.architecture() == Process::Architecture::x86)
		{
			u32 begin{};
			process.read(ramPtr.value(), &begin, sizeof(begin));
			return static_cast<std::uintptr_t>(begin);
		}
		else
		{
			u64 begin{};
			process.read(ramPtr.value(), &begin, sizeof(begin));
			return static_cast<std::uintptr_t>(begin);
		}
	}

	std::uintptr_t ramAGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
	{
		const auto bufferSize{ op.pattern.size() };
		Buffer buffer(bufferSize);
		auto* const bufferPtr{ buffer.data() };
		auto* const patternPtr{ op.pattern.data() };

		while (running)
		{
			const auto ramVal{ DuckStation::ramVal(process).value_or(0) };

			if (ramVal)
			{
				process.read(ramVal + op.offset, bufferPtr, bufferSize);
				if (std::memcmp(bufferPtr, patternPtr, bufferSize) == 0)
				{
					return ramVal;
				}
			}
			std::this_thread::sleep_for(g_settings.searchGameThreadSleepDelay);
		}

		return AsyncGameSearcher::exitValue;
	}
}
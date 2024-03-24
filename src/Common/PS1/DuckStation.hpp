#pragma once

#include "Common/GameInfo.hpp"
#include "Common/Process.hpp"
#include "Common/Types.hpp"

#include <atomic>
#include <optional>

namespace PS1::DuckStation
{
	std::optional<std::uintptr_t> ramPtr(const Process& process);
	std::optional<std::uintptr_t> ramVal(const Process& process);
	std::uintptr_t ramAGS(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);
}
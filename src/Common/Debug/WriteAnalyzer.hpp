#pragma once

#if EF_DEBUG
#include "Common/Types.hpp"

class Ram;

namespace Debug::WriteAnalyzer
{
	void drawWindow(bool* isOpen);
	void update(const Ram& ram, std::uintptr_t offset, void* data, std::size_t size);
}
#endif
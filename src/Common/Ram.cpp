#include "Ram.hpp"

#include "Util.hpp"

#include <type_traits>

Ram::Ram(std::shared_ptr<Process> process, std::unique_ptr<RamRW> rw, std::size_t size)
	: m_process(std::move(process)), m_rw(std::move(rw)), m_size(size)
{
}

bool Ram::isPatternValid(std::uintptr_t offset, std::span<const u8> pattern) const
{
	return Util::isValidProcessPatternOffset(*m_process, begin() + offset, pattern);
}

const Process& Ram::process() const
{
	return *m_process;
}

std::uintptr_t Ram::begin() const
{
	return m_rw->begin();
}

std::size_t Ram::size() const
{
	return m_size;
}
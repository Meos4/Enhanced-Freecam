#include "Ram.hpp"

#include "Util.hpp"

#include <type_traits>

Ram::Ram(std::shared_ptr<Process> process, std::uintptr_t begin, std::size_t size, bool writeNoProtect)
	: m_process(std::move(process)), m_begin(begin), m_size(size),
	m_writeCb(writeNoProtect ? &Process::writeNoProtect : &Process::write)
{
}

bool Ram::isPatternValid(std::uintptr_t offset, std::span<const u8> pattern) const
{
	return Util::isValidProcessPatternOffset(*m_process, m_begin + offset, pattern);
}

const Process& Ram::process() const
{
	return *m_process;
}

std::uintptr_t Ram::begin() const
{
	return m_begin;
}

std::size_t Ram::size() const
{
	return m_size;
}
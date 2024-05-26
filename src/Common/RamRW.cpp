#include "RamRW.hpp"

RamRWEmulator::RamRWEmulator(Process* process, std::uintptr_t begin, bool writeNoProtect)
	: m_process(process), m_begin(begin), m_writeCb(writeNoProtect ? &Process::writeNoProtect : &Process::write)
{
}

void RamRWEmulator::read(std::uintptr_t offset, void* val, std::size_t size)
{
	m_process->read(m_begin + offset, val, size);
}

void RamRWEmulator::write(std::uintptr_t offset, void* val, std::size_t size)
{
	(m_process->*m_writeCb)(m_begin + offset, val, size);
}

std::uintptr_t RamRWEmulator::begin() const
{
	return m_begin;
}
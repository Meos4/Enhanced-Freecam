#pragma once

#include "Process.hpp"
#include "Types.hpp"

class RamRW
{
public:
	virtual ~RamRW() = default;

	virtual void read(std::uintptr_t offset, void* val, std::size_t size) = 0;
	virtual void write(std::uintptr_t offset, void* val, std::size_t size) = 0;
	virtual std::uintptr_t begin() const = 0;
};

class RamRWEmulator final : public RamRW
{
public:
	RamRWEmulator(Process* process, std::uintptr_t begin, bool writeNoProtect);

	void read(std::uintptr_t offset, void* val, std::size_t size) override;
	void write(std::uintptr_t offset, void* val, std::size_t size) override;

	std::uintptr_t begin() const override;
private:
	Process* m_process;
	std::uintptr_t m_begin;
	void (Process::*m_writeCb)(std::uintptr_t, void*, std::size_t) const;
};
#pragma once

#include "GameInfo.hpp"
#include "Process.hpp"

#include <atomic>
#include <future>
#include <memory>
#include <optional>

using AGSCallback = std::uintptr_t(*)(const Process&, const std::atomic<bool>&, const OffsetPattern&);

class AsyncGameSearcher final
{
public:
	static constexpr auto exitValue{ std::uintptr_t(-1) };

	AsyncGameSearcher(std::shared_ptr<Process> process, const OffsetPattern& op, AGSCallback cb);
	~AsyncGameSearcher();

	static std::uintptr_t commonSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op);
	static std::uintptr_t emulatorSearcher
		(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op, std::size_t pageSizeMin);

	std::optional<std::uintptr_t> get();
	void stop();
private:
	std::uintptr_t run();

	std::shared_ptr<Process> m_process;
	std::atomic<bool> m_running{ true };
	OffsetPattern m_op;
	AGSCallback m_cb;
	std::future<std::uintptr_t> m_future;
};
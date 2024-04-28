#include "AsyncGameSearcher.hpp"

#include "Buffer.hpp"
#include "Util.hpp"

#include <ranges>

AsyncGameSearcher::AsyncGameSearcher(std::shared_ptr<Process> process, const OffsetPattern& op, AGSCallback cb)
	: m_process(process), m_op(op), m_cb(cb),
	m_future(std::async(std::launch::async, [this]() { return this->run(); }))
{
}

AsyncGameSearcher::~AsyncGameSearcher()
{
	if (m_future.valid())
	{
		m_running = false;
		m_future.wait();
	}
}

std::uintptr_t AsyncGameSearcher::commonSearcher(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op)
{
	Buffer buffer;

	while (1)
	{
		for (const auto& [begin, size] : std::views::reverse(process.virtualMemoryPages()))
		{
			if (!running)
			{
				return AsyncGameSearcher::exitValue;
			}

			buffer.resize(size);
			process.read(begin, buffer.data(), size);
			const auto it{ Util::findBufferPatternIterator(buffer, op.pattern) };

			if (it != buffer.end())
			{
				const auto distance{ std::distance(static_cast<Buffer::const_iterator>(buffer.begin()), it) };
				return begin + distance - op.offset;
			}
		}
	}
}

std::uintptr_t AsyncGameSearcher::emulatorSearcher
	(const Process& process, const std::atomic<bool>& running, const OffsetPattern& op, std::size_t pageSizeMin)
{
	Buffer buffer;

	while (1)
	{
		for (const auto& [begin, size] : Util::createContiguousVmp(process.virtualMemoryPages(), pageSizeMin))
		{
			if (!running)
			{
				return AsyncGameSearcher::exitValue;
			}

			const auto offset{ begin + op.offset };
			const auto bufferSize{ size - op.offset };
			buffer.resize(bufferSize);
			process.read(offset, buffer.data(), bufferSize);
			const auto it{ Util::findBufferPatternIterator(buffer, op.pattern) };

			if (it != buffer.end())
			{
				const auto distance{ std::distance(static_cast<Buffer::const_iterator>(buffer.begin()), it) };
				return offset + distance - op.offset;
			}
		}
	}
}

std::optional<std::uintptr_t> AsyncGameSearcher::get()
{
	if (m_future.wait_for(std::chrono::milliseconds{ 0 }) == std::future_status::ready)
	{
		return m_running ? m_future.get() : AsyncGameSearcher::exitValue;
	}
	return std::nullopt;
}

void AsyncGameSearcher::stop()
{
	m_running = false;
}

std::uintptr_t AsyncGameSearcher::run()
{
	return m_cb(*m_process, m_running, m_op);
}
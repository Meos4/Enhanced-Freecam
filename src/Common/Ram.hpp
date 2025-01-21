#pragma once

#if EF_DEBUG
#include "Debug/WriteAnalyzer.hpp"
#endif

#include "Buffer.hpp"
#include "Process.hpp"
#include "RamRW.hpp"
#include "Types.hpp"

#include <cstring>
#include <memory>
#include <span>
#include <tuple>

class Ram final
{
public:
	Ram(std::shared_ptr<Process> process, std::unique_ptr<RamRW> rw, std::size_t size);

	bool isPatternValid(std::uintptr_t offset, std::span<const u8> pattern) const;

	const Process& process() const;
	std::uintptr_t begin() const;
	std::size_t size() const;

	template <typename T>
	T read(std::uintptr_t offset, std::size_t size = sizeof(T)) const
	{
		T val;
		read(offset, &val, size);
		return val;
	}

	template <typename T>
	void read(std::uintptr_t offset, T* val, std::size_t size = sizeof(T)) const
	{
		m_rw->read(offset, val, size);
	}

	template <typename... Args>
	std::tuple<Args...> readPacket(std::uintptr_t offset) const
	{
		static constexpr auto size{ (sizeof(Args) + ...) };
		SBuffer<size> buffer{};
		auto* bufferPtr{ buffer.data() };
		read(offset, bufferPtr, size);

		auto impl = [&](auto* v)
		{
			std::memcpy(v, bufferPtr, sizeof(*v));
			bufferPtr += sizeof(*v);
		};

		std::tuple<Args...> tuple;
		std::apply([&impl](auto&... args) { (impl(&args), ...); }, tuple);
		return tuple;
	}

	template <typename T>
	void write(std::uintptr_t offset, const T& val, std::size_t size = sizeof(T)) const
	{
		auto writeIfDifferent = [&](void* bufferPtr)
		{
			read(offset, bufferPtr, size);
			if (std::memcmp(&val, bufferPtr, size) != 0)
			{
	#if EF_DEBUG
				Debug::WriteAnalyzer::update(*this, offset, (void*)&val, size);
	#endif
				m_rw->write(offset, (void*)&val, size);
			}
		};

		static constexpr auto stackMax{ 512 };

		if (size > stackMax)
		{
			Buffer buffer(size);
			writeIfDifferent(buffer.data());
		}
		else
		{
			SBuffer<stackMax> buffer;
			writeIfDifferent(buffer.data());
		}
	}

	template <typename... Args>
	void writeConditional(bool isEnabled, Args&&... args) const
	{
		static_assert((sizeof...(Args) % 3) == 0, "Arguments must be a multiple of 3 (Offset-True-False)");

		if constexpr (sizeof...(Args))
		{
			auto impl = [&]<typename Tuple, std::size_t... Is>(Tuple tup, std::index_sequence<Is...>)
			{
				(write(std::get<Is * 3>(tup), isEnabled ? std::get<Is * 3 + 1>(tup) : std::get<Is * 3 + 2>(tup)), ...);
			};
			impl(std::make_tuple(std::forward<Args>(args)...), std::make_index_sequence<sizeof...(Args) / 3>{});
		}
	}
private:
	std::shared_ptr<Process> m_process;
	std::unique_ptr<RamRW> m_rw;
	std::size_t m_size;
};
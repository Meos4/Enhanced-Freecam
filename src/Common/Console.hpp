#pragma once

#include <format>
#include <string>
#include <type_traits>

namespace Console
{
	enum class Type
	{
		Common,
		Success,
		Error,
		Exception,
		Count
	};

	void drawWindow();
	void appendInternal(Console::Type type, std::string&& log);
	bool isEmpty();
	std::string lastLog();

	template <typename... Args>
	std::string formatLog(const char* message, Args&&... args)
	{
		if constexpr (sizeof...(Args))
		{
			return std::vformat(message, std::make_format_args(args...));
		}
		else
		{
			return message;
		}
	}

	template <typename... Args>
	void append(Console::Type type, const char* message, Args&&... args)
	{
		Console::appendInternal(type, std::move(Console::formatLog(message, std::forward<Args>(args)...)));
	}

	template <typename... Args>
	void appendIfDifferent(Console::Type type, const char* message, Args&&... args)
	{
		auto log{ Console::formatLog(message, std::forward<Args>(args)...) };
		if (Console::isEmpty() || log != Console::lastLog())
		{
			Console::appendInternal(type, std::move(log));
		}
	}
}
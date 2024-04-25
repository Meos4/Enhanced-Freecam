#include "WriteAnalyzer.hpp"

#if EF_DEBUG
#include "Common/Console.hpp"
#include "Common/Ram.hpp"
#include "Common/Ui.hpp"

#include <array>

namespace Debug::WriteAnalyzer
{
	using Id_t = u32;

	enum : Id_t
	{
		VALID_RANGE = 1 << 0,
		DISABLED_VERIFICATION = 1 << 1,
		MULTIPLE_OF_4096 = 1 << 2
	};

	static constexpr auto count{ 3 };

	static WriteAnalyzer::Id_t wa{ WriteAnalyzer::VALID_RANGE };

	void drawWindow(bool* isOpen)
	{
		ImGui::Begin("Write Analyzer", isOpen);

		static constexpr std::array<const char*, WriteAnalyzer::count> names
		{
			"Valid Range", 
			"Disabled Verification", 
			"Multiple of 4096"
		};

		for (s32 i{}; i < WriteAnalyzer::count; ++i)
		{
			const auto bit{ 1 << i };
			bool isEnabled{ (wa & bit) ? true : false };
			if (Ui::checkbox(names[i], &isEnabled))
			{
				wa ^= bit;
			}
		}

		ImGui::End();
	}

	void update(const Ram& ram, std::uintptr_t offset, void* data, std::size_t size)
	{
		if (wa & WriteAnalyzer::VALID_RANGE)
		{
			const auto ramSize{ ram.size() };
			if (offset + size > ramSize)
			{
				Console::append(Console::Type::Error, "Valid Range: {:X}+{:X} overflow {:X}", offset, size, ramSize);
			}
		}
		if (wa & WriteAnalyzer::DISABLED_VERIFICATION)
		{
			Console::append(Console::Type::Error, "Disabled Verification: {:X} (size {})", offset, size);
		}
		if (wa & WriteAnalyzer::MULTIPLE_OF_4096)
		{
			if (offset % 4096 == 0)
			{
				Console::append(Console::Type::Error, "Multiple of 4096: {:X}", offset);
			}
		}
	}
}
#endif
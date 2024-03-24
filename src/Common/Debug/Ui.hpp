#pragma once

#if EF_DEBUG
#include "Common/Buffer.hpp"
#include "Common/Ram.hpp"
#include "Common/TemplateTypes.hpp"
#include "Common/Ui.hpp"

#include <format>
#include <utility>

namespace Debug::Ui
{
	void mipsCallWindow(const Ram& ram, bool* isOpen);
	void patternTextBoxTTY();

	template <std::size_t Size>
	constexpr std::pair<const char*, const char*> hexFormat()
	{
		return { "{:02X}", "00" };
	}

	template <>
	constexpr std::pair<const char*, const char*> hexFormat<2>()
	{
		return { "{:04X}", "0000" };
	}

	template <>
	constexpr std::pair<const char*, const char*> hexFormat<4>()
	{
		return { "{:08X}", "00000000" };
	}

	template <>
	constexpr std::pair<const char*, const char*> hexFormat<8>()
	{
		return { "{:016X}", "0000000000000000" };
	}

	template <Integral T>
	void offsetWindow(T* offset, std::size_t size, const Ram& ram, bool* isOpen)
	{
		const auto xSpacingPrevious{ ::Ui::xSpacing() };
		auto [format, xSpacingStr]{ Debug::Ui::hexFormat<sizeof(T)>() };

		ImGui::Begin("Offset", isOpen);

		const auto xSpacing{ ::Ui::xSpacingStr(xSpacingStr) };
		::Ui::setXSpacingNoMin(xSpacing);

		static s32 byteSize{};
		::Ui::slider(::Ui::lol("Byte"), &byteSize, "%d", ImGuiSliderFlags_None, 0, 32);

		Buffer buffer(byteSize);
		auto* const bufferPtr{ buffer.data() };

		const auto byteWidth{ ImGui::CalcTextSize("FFF").x };

		for (std::size_t i{}; i < size; ++i)
		{
			const auto offsetFormatted{ std::vformat(format, std::make_format_args(offset[i])) };

			if (byteSize)
			{
				::Ui::labelXSpacing(offsetFormatted.c_str());
				ram.read(offset[i], bufferPtr, byteSize);

				for (std::size_t j{}; j < byteSize; ++j)
				{
					ImGui::Text("%02X", buffer[j]);

					if (j != byteSize - 1)
					{
						const auto extra{ (j + 1) / 4 };
						ImGui::SameLine(xSpacing + byteWidth * (j + 1) + byteWidth / 3.f * extra);
					}
				}
			}
			else
			{
				ImGui::Text(offsetFormatted.c_str());
			}
		}

		ImGui::End();
		::Ui::setXSpacing(xSpacingPrevious);
	}
}
#endif
#include "Ui.hpp"

#if EF_DEBUG
#include "Common/Mips.hpp"
#include "Common/Util.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace Debug::Ui
{
	void mipsCallWindow(const Ram& ram, bool* isOpen)
	{
		struct Call
		{
			u32 offset;
			Mips_t instr;
			bool isEnabled;
		};

		static struct
		{
			u32 begin{};
			u32 size{};
			std::vector<Call> call;
			std::size_t lastHovered{ std::size_t(-1) };
			s32 column{ 3 };
		} data;

		const auto ramSize{ static_cast<u32>(ram.size()) };

		auto isACallback = [](Mips_t instr)
		{
			return (instr >> 16) % 32 == 0 && (instr & 0x0000FFFF) == 0x0000F809;
		};

		auto clearCall = [&]()
		{
			for (auto& [offset, instr, isEnabled] : data.call)
			{
				if (!isEnabled)
				{
					ram.write(offset, instr);
				}
			}
			data.call.clear();
		};

		auto searchCall = [&]()
		{
			clearCall();

			const auto arraySize{ data.size / sizeof(Mips_t) };
			std::vector<Mips_t> instructions(arraySize);
			ram.read(data.begin, instructions.data(), data.size);

			static constexpr auto jalMin{ Mips::jal(0) };
			const auto jalMax{ Mips::jal(ramSize) };

			for (std::size_t i{}; i < arraySize; ++i)
			{
				const auto instr{ instructions[i] };
				if (instr >= jalMin && instr <= jalMax || isACallback(instr))
				{
					data.call.emplace_back(Call{ u32(data.begin + i * sizeof(Mips_t)), instr, true });
				}
			}
		};

		ImGui::Begin("Mips Call", isOpen);
		const auto buttonOneLetterSize{ ::Ui::buttonOneLetterSize() };

		if (::Ui::button("+", buttonOneLetterSize))
		{
			data.begin = std::clamp(data.begin + data.size, u32(0), ramSize);
			searchCall();
		}

		ImGui::SameLine();
		if (::Ui::button("-", buttonOneLetterSize))
		{
			data.begin = std::clamp(data.begin - data.size, u32(0), ramSize);
			searchCall();
		}

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
		::Ui::drag("##MCBegin", &data.begin, u32(4), "%X", 0, u32(0), ramSize);
		data.begin = std::clamp(data.begin - (data.begin % 4), u32(0), ramSize);

		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		::Ui::drag("##MCSize", &data.size, u32(4), "%X", 0, u32(0), ramSize);
		data.size = std::clamp(data.size - (data.size % 4), u32(0), ramSize - data.begin);

		if (::Ui::button("Search"))
		{
			searchCall();
		}

		if (data.call.size())
		{
			ImGui::SameLine();
			if (::Ui::button("Clear"))
			{
				clearCall();
			}

			auto enableButton = [&](const char* label, bool enable, u32 color)
			{
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Button, color);
				if (::Ui::button(label))
				{
					for (auto& [offset, instr, isEnabled] : data.call)
					{
						isEnabled = enable;
					}
				}
				ImGui::PopStyleColor();
			};

			enableButton("ON", true, ::Ui::color(::Ui::Color::GreenY));
			enableButton("OFF", false, ::Ui::color(::Ui::Color::RedX));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			::Ui::slider("##MCColumn", &data.column, "%d", ImGuiSliderFlags_AlwaysClamp, 1, 6);

			bool isOneHovered{};
			const bool isRMouseDown{ ImGui::GetIO().MouseDown[1] };
			const auto callButtonSize{ ImGui::CalcTextSize("AAAAAAAA -> AAAAAAAA") };
			const auto callSize{ data.call.size() };

			auto* const style{ &ImGui::GetStyle() };
			const auto oldButtonTextAlign{ style->ButtonTextAlign };
			style->ButtonTextAlign = { 0.f, 0.5f };

			for (std::size_t i{}; i < callSize; i += data.column)
			{
				for (s32 j{}; j < data.column && (i + j) < callSize; ++j)
				{
					auto* const c{ &data.call[i + j] };
					std::string formattedOffset;

					if (isACallback(c->instr))
					{
						formattedOffset = std::format("{:08X} -> Callback", c->offset);
					}
					else
					{
						formattedOffset = std::format("{:08X} -> {:08X}", c->offset, (c->instr & ~0x0C000000) << 2);
					}

					::Ui::pushStyleColor(c->isEnabled ? ::Ui::color(::Ui::Color::GreenY) : ::Ui::color(::Ui::Color::RedX), ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActive);
					if (::Ui::button(formattedOffset.c_str(), { callButtonSize.x, 0.f }))
					{
						c->isEnabled = !c->isEnabled;
					}
					ImGui::PopStyleColor(3);

					if (ImGui::IsItemHovered())
					{
						isOneHovered = true;
						if (isRMouseDown && data.lastHovered != i)
						{
							data.lastHovered = i;
							c->isEnabled = !c->isEnabled;
						}
					}

					ram.write(c->offset, c->isEnabled ? c->instr : 0x00000000);

					if (j != data.column - 1 && (i + j) < callSize - 1)
					{
						ImGui::SameLine();
					}
				}
			}

			style->ButtonTextAlign = oldButtonTextAlign;

			if (!isOneHovered || !isRMouseDown)
			{
				data.lastHovered = -1;
			}
		}

		ImGui::End();
	}

	void patternTextBoxTTY()
	{
		static std::string string;
		static constexpr auto stringSize{ 2048 };
		string.resize(stringSize);

		const auto& style{ ImGui::GetStyle() };
		const auto itemWidth{ ImGui::CalcItemWidth() };
		const auto ttyWidth{ ImGui::CalcTextSize("TTY").x };

		ImGui::InputTextMultiline("##TextPTB", string.data(), stringSize, { itemWidth - ttyWidth - style.ItemSpacing.x * 2.f, 30.f }, ImGuiInputTextFlags_CharsUppercase);
		
		ImGui::SameLine();
		if (::Ui::button("TTY##PTB"))
		{
			const auto buffer{ Util::cstrToBufferByte(string.data()) };
			const auto bufferSize{ buffer.size() };

			for (std::size_t i{}; i < bufferSize; ++i)
			{
				std::cout << +buffer[i];
				
				if (i != bufferSize - 1)
				{
					std::cout << ", ";
				}
			}

			std::cout << std::endl;
		}
	}
}
#endif
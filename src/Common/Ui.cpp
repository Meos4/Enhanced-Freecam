#include "Ui.hpp"

#include "Settings.hpp"

#include <array>

namespace Ui
{
	static auto xSpacingVar{ 0.f };
	static std::string labelOnLeftTemp;

	float xSpacing()
	{
		return Ui::xSpacingVar;
	}

	void setXSpacing(float x)
	{
		static constexpr auto min{ 100.f };
		xSpacingVar = x > min ? x : min;
	}

	void setXSpacingNoMin(float x)
	{
		xSpacingVar = x;
	}

	float xSpacingStr(const char* str)
	{
		const auto& style{ ImGui::GetStyle() };
		return ImGui::CalcTextSize(str).x + style.ItemSpacing.x * 2.f + style.WindowPadding.x;
	}

	void setXSpacingStr(const char* str)
	{
		Ui::setXSpacing(Ui::xSpacingStr(str));
	}

	void enableNextItemXSpacing()
	{
		ImGui::SetCursorPosX(xSpacingVar);
		ImGui::SetNextItemWidth(-1);
	}

	void labelXSpacing(const char* label)
	{
		ImGui::Text(label);
		ImGui::SameLine();
		Ui::enableNextItemXSpacing();
	}

	const char* lol(const char* label)
	{
		std::string labelWithoutExtra{ label };
		const auto extraPos{ labelWithoutExtra.find("##") };
		if (extraPos != std::string::npos)
		{
			labelWithoutExtra.resize(extraPos);
		}

		Ui::labelXSpacing(labelWithoutExtra.c_str());
		Ui::labelOnLeftTemp = std::format("##{}", label);
		return Ui::labelOnLeftTemp.c_str();
	}

	u32 color(Ui::Color color)
	{
		static constexpr std::array<u32, static_cast<std::size_t>(Ui::Color::Count)> colors
		{
			// A-B-G-R
			0xC0'1F'1F'C9, // Red X
			0xC0'45'AD'42, // Green Y
			0xC0'CE'70'3E, // Blue Z
			0xFF'33'E5'33, // Success
			0xFF'33'33'E5, // Error
			0xFF'33'E5'FF, // Exception
			0xFF'00'67'FF, // DuckStation
			0xFF'FA'C3'38  // PCSX2
		};

		if (!g_settings.isADarkTheme())
		{
			switch (color)
			{
			case Ui::Color::Success: return 0xFF'29'B0'29;
			case Ui::Color::Error: return 0xFF'29'29'B0;
			case Ui::Color::Exception: return 0xFF'29'60'A0;
			case Ui::Color::DuckStation: return 0xFF'00'4E'C1;
			case Ui::Color::PCSX2: return 0xFF'D1'99'05;
			}
		}

		return colors[static_cast<std::size_t>(color)];
	}

	ImVec2 buttonOneLetterSize()
	{
		const auto& style{ GImGui->Style };
		return { 16.f, 13.f + style.FramePadding.y * 2.0f };
	}

	bool downloadFileConfirmationWindow(const char* title, const char* label, const std::filesystem::path& path)
	{
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_FirstUseEver, { 0.5f, 0.5f });
		ImGui::Begin(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::TextUnformatted(label);
		ImGui::Text("Path: %s", path.parent_path().string().c_str());
		ImGui::Text("File: %s", path.filename().string().c_str());
		ImGui::Separator();
		const auto buttonSize{ 120.f };
		const auto shift{ (ImGui::GetContentRegionAvail().x - (buttonSize + ImGui::GetStyle().FramePadding.x * 2.0f)) * 0.5f };
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + shift);
		const bool okPressed{ Ui::button("OK", { 120.f, 0.f }) };
		ImGui::End();
		return okPressed;
	}

	void hoveredTooltip(const char* text)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(text);
			ImGui::EndTooltip();
		}
	}

	void buttonsFlags(std::span<const Ui::LabelFlag> buttons, bool readOnly)
	{
		const auto size{ buttons.size() };
		const auto buttonSize{ (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * (size - 1)) / size };

		for (std::size_t i{}; i < size; ++i)
		{
			bool* const flag{ buttons[i].flag };
			ImGui::PushStyleColor(ImGuiCol_Text, *flag ? Ui::color(Ui::Color::Success) : Ui::color(Ui::Color::Error));
			if (Ui::button(buttons[i].label, { buttonSize, 0 }))
			{
				if (!readOnly)
				{
					*flag = !*flag;
				}
			}
			ImGui::PopStyleColor();

			if (i != size - 1)
			{
				ImGui::SameLine();
			}
		}
	}

	bool sliderDegrees(const char* label, float* radians, const char* format, ImGuiSliderFlags flags, float min, float max)
	{
		*radians = Math::toDegrees(*radians);
		const bool changed{ Ui::slider(label, radians, format, flags, Math::toDegrees(min), Math::toDegrees(max))};
		*radians = Math::toRadians(*radians);
		return changed;
	}

	bool dragVec3Degrees(const char* label, Vec3<float>* vec, float speed, const char* format, ImGuiSliderFlags flags, float min, float max)
	{
		vec->x = Math::toDegrees(vec->x);
		vec->y = Math::toDegrees(vec->y);
		vec->z = Math::toDegrees(vec->z);

		const bool isValueChanged{ Ui::dragVec3(label, vec, speed, format, flags, min, max) };

		vec->x = Math::toRadians(vec->x);
		vec->y = Math::toRadians(vec->y);
		vec->z = Math::toRadians(vec->z);

		return isValueChanged;
	}

	void separatorText(const char* label)
	{
		ImGui::SeparatorText(label);
	}

	bool checkbox(const char* label, bool* flag)
	{
		return ImGui::Checkbox(label, flag);
	}

	bool button(const char* label, const ImVec2& size)
	{
		return ImGui::Button(label, size);
	}

	bool buttonItemWidth(const char* label)
	{
		return Ui::button(label, { ImGui::CalcItemWidth(), 0.f });
	}
}
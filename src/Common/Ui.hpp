#pragma once

#include "Math.hpp"
#include "TemplateTypes.hpp"
#include "Types.hpp"
#include "Vec3.hpp"

#include "imgui_internal.h"

#include <filesystem>
#include <format>
#include <numbers>
#include <span>
#include <string>

namespace Ui
{
	enum class Color
	{
		RedX,
		GreenY,
		BlueZ,
		Success,
		Error,
		Exception,
		Count
	};

	struct LabelFlag
	{
		const char* label;
		bool* flag;
	};

	template <typename T>
	struct LabelSetter
	{
		const char* label;
		T set;
	};

	float xSpacing();
	void setXSpacing(float x);
	void setXSpacingNoMin(float x);
	float xSpacingStr(const char* str);
	void setXSpacingStr(const char* str);
	void enableNextItemXSpacing();
	void labelXSpacing(const char* label);
	const char* lol(const char* label);
	u32 color(Ui::Color color);
	ImVec2 buttonOneLetterSize();
	bool downloadFileConfirmationWindow(const char* title, const char* label, const std::filesystem::path& path);
	void hoveredTooltip(const char* text);
	void buttonsFlags(std::span<const Ui::LabelFlag> buttons, bool readOnly);
	bool sliderDegrees(const char* label, float* radians, const char* format, ImGuiSliderFlags flags = ImGuiSliderFlags_None, float min = -Math::pi, float max = Math::pi);
	bool dragEulerAnglesDegrees(const char* label, float* x, float* y, float* z, float speed, s32 decimals, ImGuiSliderFlags flags = ImGuiSliderFlags_None);
	void separatorText(const char* label);
	bool checkbox(const char* label, bool* flag);
	bool button(const char* label, const ImVec2& size = ImVec2(0, 0));
	bool buttonItemWidth(const char* label);

	template <Arithmetic T>
	consteval ImGuiDataType imGuiDataTypeByType()
	{
		if (std::is_same_v<T, s8>) return ImGuiDataType_S8;
		if (std::is_same_v<T, u8>) return ImGuiDataType_U8;
		if (std::is_same_v<T, s16>) return ImGuiDataType_S16;
		if (std::is_same_v<T, u16>) return ImGuiDataType_U16;
		if (std::is_same_v<T, s32>) return ImGuiDataType_S32;
		if (std::is_same_v<T, u32>) return ImGuiDataType_U32;
		if (std::is_same_v<T, s64>) return ImGuiDataType_S64;
		if (std::is_same_v<T, u64>) return ImGuiDataType_U64;
		if (std::is_same_v<T, float>) return ImGuiDataType_Float;
		if (std::is_same_v<T, double>) return ImGuiDataType_Double;
	}

	template <Arithmetic T>
	constexpr std::string arithmeticFormat(s32 decimals)
	{
		if constexpr (FloatingPoint<T>)
		{
			return std::format("%.{}f", decimals);
		}
		else
		{
			return "%d";
		}
	}

	template <typename T>
	void buttonsSetter(const std::span<const Ui::LabelSetter<T>> ls, T* val)
	{
		const auto size{ ls.size() };
		const auto buttonSize{ (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * (size - 1)) / size };

		for (std::size_t i{}; i < size; ++i)
		{
			if (Ui::button(ls[i].label, { buttonSize, 0 }))
			{
				*val = ls[i].set;
			}

			if (i != size - 1)
			{
				ImGui::SameLine();
			}
		}
	}

	template <SameAs<ImU32, ImVec4> T, SameAs<ImGuiCol_>... Args>
	void pushStyleColor(T col, Args... idx)
	{
		(ImGui::PushStyleColor(idx, col), ...);
	}

	template <Arithmetic T>
	bool drag(const char* label, T* value, float speed, const char* format, ImGuiSliderFlags flags = ImGuiSliderFlags_None,
		T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max())
	{
		return ImGui::DragScalar(label, Ui::imGuiDataTypeByType<T>(), value, speed, &min, &max, format, flags);
	}

	template <Arithmetic T>
	bool slider(const char* label, T* value, const char* format, ImGuiSliderFlags flags = ImGuiSliderFlags_None,
		T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max())
	{
		return ImGui::SliderScalar(label, Ui::imGuiDataTypeByType<T>(), value, &min, &max, format, flags);
	}

	template <Arithmetic T>
	bool dragWrap(const char* label, T* value, float speed, const char* format, ImGuiSliderFlags flags, T min, T max)
	{
		if constexpr (Integral<T>)
		{
			using TempType = s64;
			TempType temp{ *value };

			if (Ui::drag(label, &temp, speed, format, flags, std::numeric_limits<TempType>::min(), std::numeric_limits<TempType>::max()))
			{
				*value = static_cast<T>(Math::wrap(temp, static_cast<TempType>(min), static_cast<TempType>(max)));
				*value = std::clamp(*value, min, max);
				return true;
			}
		}
		else
		{
			if (Ui::drag(label, value, speed, format, flags, std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max()))
			{
				*value = Math::wrap(*value, min, max);
				*value = std::clamp(*value, min, max);
				return true;
			}
		}

		return false;
	}

	template <Arithmetic T>
	bool dragVec3(const char* label, Vec3<T>* vec, float speed, const char* format, ImGuiSliderFlags flags = ImGuiSliderFlags_None,
		T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max())
	{
		bool isValueChanged{};
		const auto buttonOneLetterSize{ Ui::buttonOneLetterSize() };
		const auto itemInnerSpacingX{ GImGui->Style.ItemInnerSpacing.x };

		auto mono = [&](T* val, u32 color, const char* letter, const char* dragName)
		{
			Ui::pushStyleColor(color, ImGuiCol_Border, ImGuiCol_Button, ImGuiCol_ButtonHovered, ImGuiCol_ButtonActive);
			Ui::button(letter, buttonOneLetterSize);
			ImGui::PopStyleColor(4);
			ImGui::SameLine(0.f, 2.f);
			isValueChanged |= Ui::dragWrap(dragName, val, speed, format, flags, min, max);
			ImGui::PopItemWidth();
		};

		ImGui::PushID(label);
		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() - ((2.f + buttonOneLetterSize.x) * 3.f));

		mono(&vec->x, Ui::color(Ui::Color::RedX), "X", "##X");
		ImGui::SameLine(0, itemInnerSpacingX);
		mono(&vec->y, Ui::color(Ui::Color::GreenY), "Y", "##Y");
		ImGui::SameLine(0, itemInnerSpacingX);
		mono(&vec->z, Ui::color(Ui::Color::BlueZ), "Z", std::format("{}##Z", label).c_str());

		ImGui::PopID();
		return isValueChanged;
	}
}
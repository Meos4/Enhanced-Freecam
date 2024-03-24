#include "Console.hpp"

#include "Types.hpp"
#include "Ui.hpp"

#include <array>
#include <deque>

namespace Console
{
	struct TypeLog
	{
		Console::Type type;
		std::string log;
	};

	static std::deque<Console::TypeLog> typeLog;
	static std::array<bool, static_cast<std::size_t>(Console::Type::Count)> show{ true, true, true, true };
	static bool shouldScrollToBottom{};

	void Console::drawWindow()
	{
		auto colorByType = [](Console::Type type) -> ImVec4
		{
			auto toImVec4 = [](u32 color) -> ImVec4
			{
				auto _ = [color](auto shift)
				{
					return static_cast<float>(color >> shift * 8 & 0xFF) / 255.f;
				};

				return { _(0), _(1), _(2), _(3) };
			};

			switch (type)
			{
			case Console::Type::Success: return toImVec4(Ui::color(Ui::Color::Success));
			case Console::Type::Error: return toImVec4(Ui::color(Ui::Color::Error));
			case Console::Type::Exception: return toImVec4(Ui::color(Ui::Color::Exception));
			default: return ImGui::GetStyle().Colors[ImGuiCol_Text];
			}
		};

		ImGui::Begin("Console");

		if (Ui::button("Clear"))
		{
			Console::typeLog.clear();
		}

		ImGui::SameLine();
		if (ImGui::BeginPopup("Options"))
		{
			static constexpr std::array<const char*, static_cast<std::size_t>(Console::Type::Count)> names
			{
				"Common", "Success", "Error", "Exception"
			};

			for (std::size_t i{}; i < names.size(); ++i)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, colorByType(static_cast<Console::Type>(i)));
				Ui::checkbox(names[i], &Console::show[i]);
			}

			ImGui::PopStyleColor(static_cast<s32>(names.size()));
			ImGui::EndPopup();
		}

		if (Ui::button("Options"))
		{
			ImGui::OpenPopup("Options");
		}

		ImGui::Separator();
		ImGui::BeginChild("Scroll");

		for (const auto& [type, log] : Console::typeLog)
		{
			if (Console::show[static_cast<std::size_t>(type)])
			{
				ImGui::PushStyleColor(ImGuiCol_Text, colorByType(type));
				ImGui::TextWrapped(log.c_str());
				ImGui::PopStyleColor();
			}
		}

		if (Console::shouldScrollToBottom || ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.f);
			Console::shouldScrollToBottom = false;
		}

		ImGui::EndChild();
		ImGui::End();
	}

	void Console::appendInternal(Console::Type type, std::string&& log)
	{
		static constexpr std::size_t maxLog{ 50 };

		if (Console::typeLog.size() > maxLog)
		{
			Console::typeLog.pop_front();
		}

		Console::typeLog.emplace_back(Console::TypeLog{ type, log });
		Console::shouldScrollToBottom = true;
	}

	bool isEmpty()
	{
		return Console::typeLog.empty();
	}

	std::string lastLog()
	{
		return Console::typeLog.back().log;
	}
}
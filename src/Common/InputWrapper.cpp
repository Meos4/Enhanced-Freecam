#include "InputWrapper.hpp"

#include "Console.hpp"
#include "GamepadWrapper.hpp"
#include "Settings.hpp"
#include "Ui.hpp"
#include "Util.hpp"

#include <type_traits>

static constexpr auto _Input{ "Input" };

InputWrapper::InputWrapper(InputWrapper::BaseInputsCallback baseInputsCallback)
	: m_baseInputsCallback(baseInputsCallback), m_nameInputs(baseInputsCallback())
{
	if (g_settings.keyboardLayout == KeyboardLayout::Azerty)
	{
		convertQwertyToAzertyInputs();
	}
}

void InputWrapper::draw()
{
	if (Ui::button("Clear"))
	{
		for (auto& [name, inputs] : m_nameInputs)
		{
			for (auto& input : inputs)
			{
				input = std::monostate{};
			}
		}

		if (m_waitingInput.has_value())
		{
			m_waitingInput = std::nullopt;
		}
	}

	ImGui::SameLine();
	if (Ui::button("Reset"))
	{
		m_nameInputs = m_baseInputsCallback();

		if (g_settings.keyboardLayout == KeyboardLayout::Azerty)
		{
			convertQwertyToAzertyInputs();
		}

		if (m_waitingInput.has_value())
		{
			m_waitingInput = std::nullopt;
		}
	}
	ImGui::Separator();

	ImGui::BeginChild("Scroll", { 0.f, 0.f }, false, ImGuiWindowFlags_HorizontalScrollbar);
	s32 buttonID{};
	const ImVec2 buttonSize{ 120.f, 0.f };
	const auto buttonOneLetterSize{ Ui::buttonOneLetterSize() };

	auto xStringMaxSize{ 0.f };
	for (const auto& [name, inputs] : m_nameInputs)
	{
		const auto vec{ ImGui::CalcTextSize(name) };
		if (vec.x > xStringMaxSize)
		{
			xStringMaxSize = vec.x;
		}
	}

	const auto& style{ ImGui::GetStyle() };
	Ui::setXSpacing(xStringMaxSize + style.ItemSpacing.x);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 4.f, style.ItemSpacing.y });

	for (s32 id{}; id < m_nameInputs.size(); ++id)
	{
		Ui::labelXSpacing(m_nameInputs[id].name);

		for (std::size_t index{}; index < InputWrapper::nbInputs; ++index)
		{
			ImGui::PushID(buttonID++);

			if (m_waitingInput.has_value() &&
				m_waitingInput.value().id == id &&
				m_waitingInput.value().index == index)
			{
				if (Ui::button("...", buttonSize))
				{
					m_waitingInput = std::nullopt;
				}
			}
			else
			{
				if (Ui::button(toString(id, index).c_str(), buttonSize))
				{
					m_waitingInput = { id, index };
				}
			}

			ImGui::SameLine();
			if (Ui::button("X", buttonOneLetterSize))
			{
				m_waitingInput = std::nullopt;
				m_nameInputs[id].inputs[index] = std::monostate{};
			}

			if (index + 1 != m_nameInputs[id].inputs.size())
			{
				ImGui::SameLine();
			}

			ImGui::PopID();
		}
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	if (m_waitingInput.has_value())
	{
		for (Keyboard::Key_t i{}; i < static_cast<Keyboard::Key_t>(Keyboard::Key::Count); ++i)
		{
			const auto key{ static_cast<Keyboard::Key>(i) };

			if (Keyboard::isHeld(key))
			{
				auto* const mapped{ &m_nameInputs[m_waitingInput.value().id] };
				mapped->inputs[m_waitingInput.value().index] = key;
				m_waitingInput = std::nullopt;
				return;
			}
		}

		for (s32 i{}; i < static_cast<s32>(Gamepad::Action::Count); ++i)
		{
			const auto action{ static_cast<Gamepad::Action>(i) };

			for (u32 j{}; j < GamepadWrapper::gamepadMax; ++j)
			{
				if (GamepadWrapper::isHeld(action, j))
				{
					auto* const mapped{ &m_nameInputs[m_waitingInput.value().id] };
					mapped->inputs[m_waitingInput.value().index] = InputWrapper::GamepadActionId{ action, j };
					m_waitingInput = std::nullopt;
					return;
				}
			}
		}
	}
}

void InputWrapper::readSettings(const Json::Read& json)
{
	try
	{
		if (!json.contains(_Input))
		{
			return;
		}

		const auto& j{ json[_Input] };

		if (j.size() != m_nameInputs.size())
		{
			return;
		}

		for (s32 id{}; id < m_nameInputs.size(); ++id)
		{
			if (j[id].size() < InputWrapper::nbInputs)
			{
				return;
			}

			for (std::size_t index{}; index < InputWrapper::nbInputs; ++index)
			{
				const auto& input{ j[id][index] };
				const auto& variantIndex{ input.at(0) };

				if (variantIndex == 0)
				{
					m_nameInputs[id].inputs[index] = std::monostate{};
				}
				else if (variantIndex == 1)
				{
					const auto key{ input.at(1).get<Keyboard::Key>() };
					if (key < Keyboard::Key::Count)
					{
						m_nameInputs[id].inputs[index] = key;
					}
				}
				else if (variantIndex == 2)
				{
					const auto action{ input.at(1).at(0).get<decltype(InputWrapper::GamepadActionId::action)>() };
					const auto padId{ input.at(1).at(1).get<decltype(InputWrapper::GamepadActionId::id)>() };

					if (action < Gamepad::Action::Count && padId < GamepadWrapper::gamepadMax)
					{
						m_nameInputs[id].inputs[index] = InputWrapper::GamepadActionId{ action, padId };
					}
				}
			}
		}
	}
	catch (const Json::Exception& e)
	{
		Console::append(Console::Type::Exception, Json::exceptionFormat, _Input, e.what());
	}
}

void InputWrapper::writeSettings(Json::Write* json) const
{
	auto* const j{ &(*json)[_Input] };

	for (s32 id{}; id < m_nameInputs.size(); ++id)
	{
		for (std::size_t index{}; index < InputWrapper::nbInputs; ++index)
		{
			auto variantToJson = [](const InputWrapper::Types& types)
			{
				auto fn = [](const auto& val) -> Json::Write
				{
					using VariantType = std::decay_t<decltype(val)>;

					if constexpr (std::is_same_v<VariantType, std::monostate>)
					{
						return 0;
					}
					else if constexpr (std::is_same_v<VariantType, InputWrapper::GamepadActionId>)
					{
						return { val.action, val.id };
					}
					else
					{
						return val;
					}
				};

				return std::visit(fn, types);
			};

			(*j)[id][index] = { m_nameInputs[id].inputs[index].index(), variantToJson(m_nameInputs[id].inputs[index]) };
		}
	}
}

bool InputWrapper::isPressed(s32 id)
{
	for (const auto& input : m_nameInputs[id].inputs)
	{
		if (std::visit(Util::Overload
		{
			[&]([[maybe_unused]] std::monostate) { return false; },
			[&](Keyboard::Key key) { return Keyboard::isPressed(key); },
			[&](const InputWrapper::GamepadActionId& ai) { return GamepadWrapper::isPressed(ai.action, ai.id); }
		}, input))
		{
			return true;
		}
	}
	return false;
}

bool InputWrapper::isHeld(s32 id)
{
	for (const auto& input : m_nameInputs[id].inputs)
	{
		if (std::visit(Util::Overload
		{
			[&]([[maybe_unused]] std::monostate) { return false; },
			[&](Keyboard::Key key) { return Keyboard::isHeld(key); },
			[&](const InputWrapper::GamepadActionId& ai) { return GamepadWrapper::isHeld(ai.action, ai.id); }
		}, input))
		{
			return true;
		}
	}
	return false;
}

float InputWrapper::sensitivity(s32 id)
{
	auto sensitivity{ 0.f };

	for (const auto& input : m_nameInputs[id].inputs)
	{
		std::visit(Util::Overload
		{
			[&]([[maybe_unused]] std::monostate) {},
			[&](Keyboard::Key key) { if (Keyboard::isHeld(key)) { sensitivity += 1.f; } },
			[&](const InputWrapper::GamepadActionId& ai) { sensitivity += GamepadWrapper::sensitivity(ai.action, ai.id); }
		}, input);
	}
	return sensitivity;
}

std::string InputWrapper::toString(s32 id, std::size_t index) const
{
	auto keyboardBehavior = [](Keyboard::Key key) -> std::string
	{
		return g_settings.keyboardLayout == KeyboardLayout::Qwerty ? Keyboard::toStringQwerty(key) : Keyboard::toStringAzerty(key);
	};

	auto gamepadBehavior = [](const InputWrapper::GamepadActionId& ai)
	{
		std::string str{ g_settings.gamepadLayout == GamepadLayout::Xbox ? Gamepad::toStringXbox(ai.action) : Gamepad::toStringPlayStation(ai.action) };
		str += std::format(" ({})", ai.id + 1);
		return str;
	};

	return std::visit(Util::Overload
	{
		[&]([[maybe_unused]] std::monostate) { return std::string{ "N/A" }; },
		[&](Keyboard::Key key) { return keyboardBehavior(key); },
		[&](const InputWrapper::GamepadActionId& ai) { return gamepadBehavior(ai); }
	}, m_nameInputs[id].inputs[index]);
}

void InputWrapper::convertQwertyToAzertyInputs()
{
	for (auto& [name, inputs] : m_nameInputs)
	{
		for (auto& input : inputs)
		{
			if (std::holds_alternative<Keyboard::Key>(input))
			{
				input = Keyboard::qwertyToAzertyKey(std::get<Keyboard::Key>(input));
			}
		}
	}
}
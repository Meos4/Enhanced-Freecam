#pragma once

#include "Gamepad.hpp"
#include "Json.hpp"
#include "Keyboard.hpp"
#include "Types.hpp"

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

class InputWrapper final
{
public:
	struct GamepadActionId
	{
		Gamepad::Action action;
		u32 id;
	};

	using Types = std::variant<std::monostate, Keyboard::Key, GamepadActionId>;
	static constexpr std::size_t nbInputs{ 2 };

	struct NameInputs
	{
		const char* name;
		std::array<InputWrapper::Types, InputWrapper::nbInputs> inputs;
	};

	using BaseInputsCallback = std::vector<InputWrapper::NameInputs>(*)();

	struct WaitingInput
	{
		s32 id;
		std::size_t index;
	};

	InputWrapper(InputWrapper::BaseInputsCallback baseInputsCallback);

	void draw();
	void readSettings(const Json::Read& json);
	void writeSettings(Json::Write* json) const;
	bool isPressed(s32 id);
	bool isHeld(s32 id);
	float sensitivity(s32 id);
private:
	std::string toString(s32 id, std::size_t index) const;
	void convertQwertyToAzertyInputs();

	InputWrapper::BaseInputsCallback m_baseInputsCallback;
	std::vector<InputWrapper::NameInputs> m_nameInputs;
	std::optional<InputWrapper::WaitingInput> m_waitingInput;
};
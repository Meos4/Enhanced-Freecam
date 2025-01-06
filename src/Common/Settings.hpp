#pragma once

#include "PS2/PCSX2.hpp"

#include "Gamepad.hpp"
#include "InputWrapper.hpp"
#include "Json.hpp"
#include "Math.hpp"

#include <array>
#include <chrono>
#include <string>

enum class Theme
{
	Dark,
	Ruby,
	Dark2,
	Light,
	Classic,
	Count
};

enum class KeyboardLayout
{
	Qwerty,
	Azerty
};

enum class SearchMode
{
	Auto,
	Manual
};

enum class Font
{
	RobotoMedium13,
	RobotoMedium14,
	RobotoMedium15,
	Count
};

struct ImFont;

struct Settings
{
	Theme theme{ Theme::Dark };
	KeyboardLayout keyboardLayout{ KeyboardLayout::Qwerty };
	SearchMode processSearchMode{ SearchMode::Auto };
	Font font{ Font::RobotoMedium14 };
	std::array<ImFont*, static_cast<std::size_t>(Font::Count)> imFonts;
	bool smoothCamera{ true };
	bool isMultiViewports{ true };
	bool multiViewportsAlwaysOnTop{};
	bool isWelcomeModalOpen{ true };
	float invalidGameDelay{ 1.f };
	float searchProcessDelay{ 1.f };
	std::chrono::milliseconds searchGameThreadSleepDelay{ 200 };

	float deltaTimeScalar{ 60.f };
	float dragRadiansSpeed{ 0.002f };
	float dragFloatSpeed{ 0.2f };
	s32 positionFloatDecimals{ 2 };
	s32 rotationFloatDecimals{ 2 };
	float fovRadiansMin{ Math::pi / 128.f };
	float fovRadiansMax{ Math::pi / 1.25f };
	float fovRadiansScalar{ 0.04f };
	s32 fovFloatDecimals{ 2 };
	float movementSpeedDefault{ 1.f };
	float movementSpeed{ movementSpeedDefault };
	float movementSpeedMin{ 0.01f };
	float movementSpeedMax{ 50.f };
	float movementSpeedScalar{ 2.f };
	float rotationSpeedDefault{ 1.f };
	float rotationSpeed{ rotationSpeedDefault };
	float rotationSpeedMin{ 0.01f };
	float rotationSpeedMax{ 5.f };
	float rotationSpeedScalar{ 1.25f };
	float fovSpeedDefault{ 1.f };
	float fovSpeed{ fovSpeedDefault };
	float fovSpeedMin{ 0.01f };
	float fovSpeedMax{ 5.f };
	float fovSpeedScalar{ 1.25f };
	s32 freecamSettingsFloatDecimals{ 2 };

	struct
	{
		bool resetMovementSpeed{ true };
		bool resetRotationSpeed{};
		bool resetFovSpeed{};
		bool resetRollRotation{ true };
		bool disableButton{ true };
		bool disableJoystick{ true };
		bool pauseGame{};
		bool hideHud{};
	} gameSettings;

	struct
	{
		decltype(InputWrapper::NameInputs::inputs)
			toggleFreecam{ InputWrapper::GamepadActionId{ Gamepad::Action::L3, 0 }, Keyboard::Key::F },
			moveForward{ InputWrapper::GamepadActionId{ Gamepad::Action::Up, 0 }, Keyboard::Key::W },
			moveBackward{ InputWrapper::GamepadActionId{ Gamepad::Action::Down, 0 }, Keyboard::Key::S },
			moveRight{ InputWrapper::GamepadActionId{ Gamepad::Action::Right, 0 }, Keyboard::Key::D },
			moveLeft{ InputWrapper::GamepadActionId{ Gamepad::Action::Left, 0 }, Keyboard::Key::A },
			moveUp{ InputWrapper::GamepadActionId{ Gamepad::Action::L1, 0 }, Keyboard::Key::E },
			moveDown{ InputWrapper::GamepadActionId{ Gamepad::Action::L2, 0 }, Keyboard::Key::Q },
			rotateCounterclockwise{},
			rotateClockwise{},
			rotateUp{ InputWrapper::GamepadActionId{ Gamepad::Action::RightStickYPos, 0 }, Keyboard::Key::Up },
			rotateDown{ InputWrapper::GamepadActionId{ Gamepad::Action::RightStickYNeg, 0 }, Keyboard::Key::Down },
			rotateLeft{ InputWrapper::GamepadActionId{ Gamepad::Action::RightStickXNeg, 0 }, Keyboard::Key::Left },
			rotateRight{ InputWrapper::GamepadActionId{ Gamepad::Action::RightStickXPos, 0 }, Keyboard::Key::Right },
			fovIncrease{ InputWrapper::GamepadActionId{ Gamepad::Action::R2, 0 }, Keyboard::Key::R },
			fovDecrease{ InputWrapper::GamepadActionId{ Gamepad::Action::R1, 0 }, Keyboard::Key::T },
			pauseGame{ InputWrapper::GamepadActionId{ Gamepad::Action::Start, 0 }, Keyboard::Key::P },
			hideHud{ InputWrapper::GamepadActionId{ Gamepad::Action::Select, 0 }, Keyboard::Key::H },
			button{},
			joystick{},
			lJoystick{},
			rJoystick{},
			movementSpeedPos{ InputWrapper::GamepadActionId{ Gamepad::Action::Triangle, 0 } },
			movementSpeedNeg{ InputWrapper::GamepadActionId{ Gamepad::Action::Cross, 0 } },
			rotationSpeedPos{},
			rotationSpeedNeg{},
			fovSpeedPos{},
			fovSpeedNeg{},
			timescalePos{ InputWrapper::GamepadActionId{ Gamepad::Action::Square, 0 } },
			timescaleNeg{ InputWrapper::GamepadActionId{ Gamepad::Action::Circle, 0 } },
			teleportToCamera{};
	} input;

	struct
	{
		float stickDeadzone{ 0.125f };
		float triggerDeadzone{ 0.1f };
	} gamepad;

	struct
	{
		float pnachVerificationDelay{ 3.f };
		float pnachVerificationTime{};
		std::string cheatsPath{ PS2::PCSX2::defaultCheatsPath().string() };
	} pcsx2;

	void init();
	std::array<float, 3> rgbFont() const;
	void drawWindow();
	void drawPCSX2CheatsPathInputText();
	bool isADarkTheme();
	void readSettings(const Json::Read& json);
	void writeSettings(Json::Write* json) const;
	void updateTheme() const;
	void updateIsMultiViewports() const;
	void updateMultiViewportsAlwaysOnTop() const;
	void updateFont() const;

	static void setThemeDark();
	static void setThemeRuby();
};

extern Settings g_settings;
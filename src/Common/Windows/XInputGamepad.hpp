#pragma once

#include "Common/Types.hpp"
#include "Common/Vec2.hpp"

#include "Header.hpp"

#include <XInput.h>

namespace Windows
{
	class XInputGamepad final
	{
	public:
		using Button_t = decltype(XINPUT_STATE::Gamepad.wButtons);

		enum Button : Button_t
		{
			Up = XINPUT_GAMEPAD_DPAD_UP,
			Down = XINPUT_GAMEPAD_DPAD_DOWN,
			Left = XINPUT_GAMEPAD_DPAD_LEFT,
			Right = XINPUT_GAMEPAD_DPAD_RIGHT,
			Start = XINPUT_GAMEPAD_START,
			Back = XINPUT_GAMEPAD_BACK,
			Left_Thumb = XINPUT_GAMEPAD_LEFT_THUMB,
			Right_Thumb = XINPUT_GAMEPAD_RIGHT_THUMB,
			Left_Shoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
			Right_Shoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,
			A = XINPUT_GAMEPAD_A,
			B = XINPUT_GAMEPAD_B,
			X = XINPUT_GAMEPAD_X,
			Y = XINPUT_GAMEPAD_Y
		};

		static constexpr u32 gamepadIdMin{ 0 };
		static constexpr u32 gamepadMax{ XUSER_MAX_COUNT };

		XInputGamepad(u32 id);

		bool update();
		bool isHeld(XInputGamepad::Button_t buttons) const;
		Vec2<float> leftStick() const;
		Vec2<float> rightStick() const;
		float leftTrigger() const;
		float rightTrigger() const;
		
		u32 id() const;
	private:
		u32 m_id;
		XINPUT_STATE m_state{};
	};
}
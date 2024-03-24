#include "XInputGamepad.hpp"

#include "Common/Math.hpp"

namespace Windows
{
	XInputGamepad::XInputGamepad(u32 id)
		: m_id(id)
	{
	}

	bool XInputGamepad::update()
	{
		if (XInputGetState(m_id, &m_state) != ERROR_SUCCESS)
		{
			m_state = {};
			return false;
		}

		return true;
	}

	bool XInputGamepad::isHeld(XInputGamepad::Button_t buttons) const
	{
		return (m_state.Gamepad.wButtons & buttons) ? true : false;
	}

	Vec2<float> XInputGamepad::leftStick() const
	{
		return
		{
			Math::normalizeInt<float>(m_state.Gamepad.sThumbLX),
			Math::normalizeInt<float>(m_state.Gamepad.sThumbLY)
		};
	}

	Vec2<float> XInputGamepad::rightStick() const
	{
		return
		{
			Math::normalizeInt<float>(m_state.Gamepad.sThumbRX),
			Math::normalizeInt<float>(m_state.Gamepad.sThumbRY)
		};
	}

	float XInputGamepad::leftTrigger() const
	{
		return Math::normalizeInt<float>(m_state.Gamepad.bLeftTrigger);
	}

	float XInputGamepad::rightTrigger() const
	{
		return Math::normalizeInt<float>(m_state.Gamepad.bRightTrigger);
	}

	u32 XInputGamepad::id() const
	{
		return m_id;
	}
}
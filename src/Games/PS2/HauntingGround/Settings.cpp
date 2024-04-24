#include "Settings.hpp"

#include "Common/Console.hpp"
#include "Common/Ui.hpp"

namespace PS2::HauntingGround
{
	static constexpr auto _Settings{ "Settings" };

	void Settings::draw()
	{
		Ui::setXSpacingStr("Reset Movement Speed");

		Ui::separatorText("When Enabling Freecam");
		Ui::checkbox(Ui::lol("Reset Movement Speed"), &resetMovementSpeed);
		Ui::checkbox(Ui::lol("Reset Rotation Speed"), &resetRotationSpeed);
		Ui::checkbox(Ui::lol("Reset Fov Speed"), &resetFovSpeed);
		Ui::checkbox(Ui::lol("Disable Button"), &disableButton);
		Ui::checkbox(Ui::lol("Disable Left Joystick"), &disableLJoystick);
		Ui::checkbox(Ui::lol("Disable Right Joystick"), &disableRJoystick);
		Ui::checkbox(Ui::lol("Pause Game"), &pauseGame);
	}

	void Settings::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Settings))
			{
				const auto& j{ json[_Settings] };
				JSON_GET(j, resetMovementSpeed);
				JSON_GET(j, resetRotationSpeed);
				JSON_GET(j, resetFovSpeed);
				JSON_GET(j, disableButton);
				JSON_GET(j, disableLJoystick);
				JSON_GET(j, disableRJoystick);
				JSON_GET(j, pauseGame);
			}
		}
		catch (const Json::Exception& e)
		{
			Console::append(Console::Type::Exception, Json::exceptionFormat, _Settings, e.what());
		}
	}

	void Settings::writeSettings(Json::Write* json) const
	{
		auto* const j{ &(*json)[_Settings] };
		JSON_SET(j, resetMovementSpeed);
		JSON_SET(j, resetRotationSpeed);
		JSON_SET(j, resetFovSpeed);
		JSON_SET(j, disableButton);
		JSON_SET(j, disableLJoystick);
		JSON_SET(j, disableRJoystick);
		JSON_SET(j, pauseGame);
	}
}
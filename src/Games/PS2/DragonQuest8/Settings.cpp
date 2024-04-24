#include "Settings.hpp"

#include "Common/Console.hpp"
#include "Common/Ui.hpp"

namespace PS2::DragonQuest8
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
		Ui::checkbox(Ui::lol("Disable Joystick"), &disableJoystick);
		Ui::checkbox(Ui::lol("Pause Game"), &pauseGame);
		Ui::checkbox(Ui::lol("Hide Hud"), &hideHud);
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
				JSON_GET(j, disableJoystick);
				JSON_GET(j, pauseGame);
				JSON_GET(j, hideHud);
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
		JSON_SET(j, disableJoystick);
		JSON_SET(j, pauseGame);
		JSON_SET(j, hideHud);
	}
}
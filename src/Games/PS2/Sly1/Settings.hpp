#pragma once

#include "Common/Json.hpp"
#include "Common/Settings.hpp"

namespace PS2::Sly1
{
	struct Settings
	{
		bool resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool resetFovSpeed{ g_settings.gameSettings.resetFovSpeed };
		bool resetXRotation{ g_settings.gameSettings.resetRollRotation };
		bool disableButton{ g_settings.gameSettings.disableButton };
		bool disableJoystick{ g_settings.gameSettings.disableJoystick };
		bool pauseGame{ g_settings.gameSettings.pauseGame };
		bool hideHud{ g_settings.gameSettings.hideHud };

		void draw();
		void readSettings(const Json::Read& json);
		void writeSettings(Json::Write* json) const;
	};
}
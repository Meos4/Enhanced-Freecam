#pragma once

#include "Common/Json.hpp"
#include "Common/Settings.hpp"

namespace PS1::DinoCrisis
{
	struct Settings
	{
		bool resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool disableButton{ g_settings.gameSettings.disableButton };
		bool pauseGame{ g_settings.gameSettings.pauseGame };

		void draw();
		void readSettings(const Json::Read& json);
		void writeSettings(Json::Write* json) const;
	};
}
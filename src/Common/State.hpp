#pragma once

#include "Json.hpp"

class GameLoop;

namespace State
{
	void drawWindow();
	void update();
	void destroy();
	GameLoop* game();
	void readSettings(const Json::Read& json);
	void writeSettings(Json::Write* json);
}
#pragma once

#include "Common/Types.hpp"
#include "Common/Vec2.hpp"

#include <optional>

namespace SDL::Util
{
	s32 nbMonitors();
	std::optional<Vec2<s32>> resolution(s32 monitor);
}
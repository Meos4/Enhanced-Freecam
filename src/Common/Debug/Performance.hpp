#pragma once

#if EF_DEBUG
#define DEBUG_PERFORMANCE_DRAW Debug::Performance::draw()
#define DEBUG_PERFORMANCE_UPDATE Debug::Performance::update()
#define DEBUG_PERFORMANCE_BEGIN(label) Debug::Performance::begin(label)
#define DEBUG_PERFORMANCE_END(label) Debug::Performance::end(label)

namespace Debug::Performance
{
	void draw();
	void update();
	void begin(const char* label);
	void end(const char* label);
}

#else
#define DEBUG_PERFORMANCE_DRAW
#define DEBUG_PERFORMANCE_UPDATE
#define DEBUG_PERFORMANCE_BEGIN(label)
#define DEBUG_PERFORMANCE_END(label)
#endif
#pragma once

#include "Ram.hpp"
#include "TemplateTypes.hpp"
#include "Types.hpp"

#include "imgui.h"

#include <array>
#include <memory>
#include <span>

#define ADD_GAME_INFO(Namespace)	\
	Namespace::Game::name,			\
	Namespace::Version::Count,		\
	Namespace::Game::versionText,	\
	Namespace::Game::offsetPattern,	\
	Namespace::Game::createLoop

#define DRAW_GAME_WINDOWS(Freecam, Controls, Settings, Bonus)\
	ImGui::Begin("Freecam"); Freecam; ImGui::End();			\
	ImGui::Begin("Controls"); Controls; ImGui::End();		\
	ImGui::Begin("Settings"); Settings; ImGui::End();		\
	ImGui::Begin("Bonus"); Bonus; ImGui::End();

template <Integral Offset, std::size_t Size>
struct OffsetPatternStatic
{
	Offset offset;
	std::array<u8, Size> pattern;
};

struct OffsetPattern
{
	std::uintptr_t offset;
	std::span<const u8> pattern;
};

class GameLoop
{
public:
	virtual ~GameLoop() = default;

	virtual void draw() = 0;
	virtual void update() = 0;
	virtual bool isValid() = 0;
};

struct GameInfo
{
	const char* name;
	s32 count;
	const char* (*versionText)(s32);
	OffsetPattern (*offsetPattern)(s32);
	std::unique_ptr<GameLoop> (*createLoop)(Ram, s32);
};

struct EmulatorInfo
{
	const char* name;
	const char* pattern;
};
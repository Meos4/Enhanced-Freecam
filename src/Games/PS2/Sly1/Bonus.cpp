#include "Bonus.hpp"

#include "Common/Buffer.hpp"
#include "Common/Console.hpp"
#include "Common/MiscModel.hpp"
#include "Common/Ui.hpp"

#include "Game.hpp"

namespace PS2::Sly1
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("No Motion Blur");

		Ui::checkbox(Ui::lol("No Motion Blur"), &m_noMotionBlur);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("Invulnerable"), &m_invulnerable);
		MiscModel::drawEnableButton("Keys", "Unlock All##Keys", &m_shouldUnlockAllKeys);
		MiscModel::drawEnableButton("Power-ups", "Unlock All##Power-ups", &m_shouldUnlockAllPowerUps);
	}

	void Bonus::update()
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };
		const auto version{ m_game->version() };

		ram.write(offset.Fn_BlendPrevFrame__Fv + 0x15C, m_noMotionBlur ? 0x00003021 : 0x24060030);
		ram.write<s32>(offset.g_fInvulnerable, m_invulnerable ? 1 : 0);

		enum : s32
		{
			LEVEL_PROGRESS_AVAILABLE = 1 << 0,
			LEVEL_PROGRESS_KEY = 1 << 1,
			LEVEL_PROGRESS_VAULT = 1 << 2,
			LEVEL_PROGRESS_SPRINT = 1 << 3
		};

		u32 worldLevelIdShift,
			levelFlagsShift,
			levelSize,
			hideoutPtrShift;

		if (version == Version::NtscU)
		{
			worldLevelIdShift = 0x20;
			hideoutPtrShift = 0x260;
		}
		else if (version == Version::Pal)
		{
			worldLevelIdShift = 0xA0;
			hideoutPtrShift = 0x570;
		}
		else
		{
			worldLevelIdShift = 0x40;
			hideoutPtrShift = 0x570;
		}

		levelFlagsShift = worldLevelIdShift + 8;
		levelSize = worldLevelIdShift + 0xC;

		auto getLevelInfos = [&]()
		{
			const auto bufferSize{ 46 * levelSize };
			Buffer levelInfos(bufferSize);
			ram.read(offset.levelInfo, levelInfos.data(), bufferSize);
			return levelInfos;
		};

		auto worldFlagNumber = [&](u8* ptr, s32 world, s32 flag)
		{
			s32 number{};
			for (s32 i{}; i < 46; ++i)
			{
				const auto levelIndex{ levelSize * i };
				if (*(s32*)(ptr + levelIndex + worldLevelIdShift) >> 8 == world)
				{
					if (*(s32*)(ptr + levelIndex + levelFlagsShift) & flag)
					{
						++number;
					}
				}
			}
			return number;
		};

		auto enableLevelsFlags = [&](u8* ptr, s32 flags)
		{
			auto allowedWorldLevelFlags = [&](s32 world, s32 level)
			{
				const s32 worldLevel{ (world << 8) + level };
				for (s32 i{}; i < 46; ++i)
				{
					const auto levelIndex{ levelSize * i };
					if (*(s32*)(ptr + levelIndex + worldLevelIdShift) == worldLevel)
					{
						return *(s32*)(ptr + levelIndex + levelFlagsShift);
					}
				}
				return 0;
			};

			const auto g_gsCur{ offset.g_gsCur };
			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ g_gsCur + (world  * 0x44C) };
				for (s32 level{}; level < 9; ++level)
				{
					const auto offsetProgress{ worldOffset + 0x10 + (0x78 * level) };
					ram.write(offsetProgress, ram.read<s32>(offsetProgress) | (allowedWorldLevelFlags(world, level) & flags));
				}
			}
		};

		auto enableLevelsAvailability = [&]()
		{
			const auto g_gsCur{ offset.g_gsCur };
			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ g_gsCur + (world * 0x44C) };
				for (s32 level{}; level < 9; ++level)
				{
					const auto offsetProgress{ worldOffset + 0x10 + (0x78 * level) };
					ram.write(offsetProgress, ram.read<s32>(offsetProgress) | LEVEL_PROGRESS_AVAILABLE);
				}
			}
		};

		if (m_shouldUnlockAllKeys)
		{
			const auto g_gsCur{ offset.g_gsCur };
			auto levelInfos{ getLevelInfos() };
			auto* const ptr{ levelInfos.data() };

			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ g_gsCur + (world  * 0x44C) };
				ram.write<s32>(worldOffset + 0x448, worldFlagNumber(ptr, world, LEVEL_PROGRESS_KEY)); // World Keys
				ram.write<s32>(worldOffset + 0x458, 1); // World open
			}

			enableLevelsAvailability();
			enableLevelsFlags(ptr, LEVEL_PROGRESS_KEY);

			const auto hideoutPtr{ ram.read<u32>(offset.g_prompt + hideoutPtrShift) };
			if (hideoutPtr)
			{
				ram.write<s32>(hideoutPtr + 0x2FC, 5);
			}

			Console::append(Console::Type::Success, "Keys unlocked successfully");
			m_shouldUnlockAllKeys = false;
		}

		if (m_shouldUnlockAllPowerUps)
		{
			const auto g_gsCur{ offset.g_gsCur };
			auto levelInfos{ getLevelInfos() };
			auto* const ptr{ levelInfos.data() };

			for (s32 world{ 1 }; world < 6; ++world)
			{
				const auto worldOffset{ g_gsCur + (world * 0x44C) };
				ram.write(worldOffset + 0x44C, worldFlagNumber(ptr, world, LEVEL_PROGRESS_VAULT)); // World Vaults
			}

			ram.write<s32>(g_gsCur + 0x19F0, -1); // Power-ups
			enableLevelsFlags(ptr, LEVEL_PROGRESS_VAULT);

			Console::append(Console::Type::Success, "Power-ups unlocked successfully");
			m_shouldUnlockAllPowerUps = false;
		}
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noMotionBlur);
				JSON_GET(j, m_invulnerable);
			}
		}
		catch (const Json::Exception& e)
		{
			Console::append(Console::Type::Exception, Json::exceptionFormat, _Bonus, e.what());
		}
	}

	void Bonus::writeSettings(Json::Write* json)
	{
		auto* const j{ &(*json)[_Bonus] };
		JSON_SET(j, m_noMotionBlur);
		JSON_SET(j, m_invulnerable);
	}
}
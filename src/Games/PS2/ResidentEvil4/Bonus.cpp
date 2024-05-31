#include "Bonus.hpp"

#include "Common/Console.hpp"
#include "Common/Mips.hpp"
#include "Common/Ui.hpp"

#include "Camera.hpp"
#include "Game.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	static constexpr auto _Bonus{ "Bonus" };

	Bonus::Bonus(Game* game)
		: m_game(game)
	{
	}

	void Bonus::draw()
	{
		Ui::setXSpacingStr("Teleport To Camera");

		Ui::checkbox(Ui::lol("No Fog"), &m_noFog);
		Ui::checkbox(Ui::lol("No Game Over"), &m_noGameOver);

		Ui::separatorText("Cheats");
		Ui::checkbox(Ui::lol("No Collisions"), &m_noCollisions);
		Ui::labelXSpacing("Unlock All");
		if (Ui::buttonItemWidth("Set"))
		{
			m_unlockAll = true;
		}
		Ui::labelXSpacing("Teleport To Camera");
		if (Ui::buttonItemWidth("Set##TTC"))
		{
			m_teleportToCamera = true;
		}
	}

	void Bonus::update(const Camera& camera)
	{
		const auto& ram{ m_game->ram() };
		const auto& offset{ m_game->offset() };

		ram.write(offset.Fn_drawFog, m_noFog ? Mips::jrRaNop() : std::array<Mips_t, 2>{ 0x27BDFF30, 0x7FB000C0 });
		ram.write(offset.Fn_updateGameOver + 0x70, m_noGameOver ? 0x00009021 : 0x0062900A);
		ram.write(offset.Fn_updatePlayer + 0x4B0, m_noCollisions ? 0x00000000 : Mips::jal(offset.Fn_updatePlayerCollisions));

		if (m_unlockAll)
		{
			const auto current{ ram.read<s32>(offset.progression) };
			ram.write(offset.progression, current | 0xFF'DD'00'0F);
			ram.write(offset.progression + 9, u8(4)); // Ada's report
			Console::append(Console::Type::Success, "All unlocked successfully");
			m_unlockAll = false;
		}

		if (m_teleportToCamera)
		{
			const auto playerPtr{ ram.read<u32>(offset.playerPtr) };

			if (playerPtr)
			{
				const auto& [px, py, pz]{ camera.position() };
				const auto& [rx, ry, rz]{ camera.rotation() };

				const auto
					sx{ std::sin(rx) },
					cx{ std::cos(rx) },
					sy{ std::sin(ry) },
					cy{ std::cos(ry) };

				static constexpr auto forwardAmount{ 1500.f };
				const Vec3<float> playerPosition
				{
					px - cx * sy * forwardAmount, 
					py - -sx * forwardAmount - 1000.f, 
					pz - cy * cx * forwardAmount
				};

				ram.write(playerPtr + 0xC0, playerPosition);
			}
			m_teleportToCamera = false;
		}
	}

	void Bonus::readSettings(const Json::Read& json)
	{
		try
		{
			if (json.contains(_Bonus))
			{
				const auto& j{ json[_Bonus] };
				JSON_GET(j, m_noFog);
				JSON_GET(j, m_noGameOver);
				JSON_GET(j, m_noCollisions);
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
		JSON_SET(j, m_noFog);
		JSON_SET(j, m_noGameOver);
		JSON_SET(j, m_noCollisions);
	}
}
#include "CustomCode.hpp"

#include "Common/Mips.hpp"

#include "Game.hpp"

namespace PS2::ResidentEvil4::CustomCode
{
	using Fn_SetPacket = std::array<Mips_t, 22>;
	using Fn_SetFov = std::array<Mips_t, 5>;

	void update(const Game& game)
	{
		const auto& ram{ game.ram() };
		const auto& offset{ game.offset() };

		const auto
			li32_cameraPtr{ Mips::li32(Mips::Register::t1, offset.cameraPtr) },
			li32_packet{ Mips::li32(Mips::Register::t2, CustomCode::packetOffset(game)) },
			li32_fov{ Mips::li32(Mips::Register::t0, CustomCode::fovOffset(game)) };

		const CustomCode::Fn_SetPacket Fn_setPacket
		{
			0x27BDFFF0, // addiu sp, -0x10
			li32_cameraPtr[0],
			li32_cameraPtr[1],
			0x8D290000, // lw t1, 0(t1)
			0x1120000F, // beqz t1, +15
			0x7FBF0000, // sq ra,(sp)
			0x01202021, // move a0, t1
			li32_packet[0],
			li32_packet[1],
			0x01402821, // move a1, t2
			0x24840080, // addiu a0, 0x80
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 0x80),
			0x01202021, // move a0, t1
			0x01402821, // move a1, t2
			0x24840140, // addiu a0, 0x140
			0x24A50080, // addiu a1, 0x80
			Mips::jal(offset.Fn_memcpy),
			Mips::li(Mips::Register::a2, 0x50),
			0x7BBF0000, // lq ra,(sp)
			0x03E00008, // jr ra
			0x27BD0010  // addiu sp, 0x10
		};

		const CustomCode::Fn_SetFov Fn_setFov
		{
			li32_fov[0],
			li32_fov[1],
			0xC50C0000, // lwc1 f12, 0(t0)
			Mips::j(offset.Fn_unknown),
			0xE62C0124 // swc1 f12, 0x124(s1)
		};

		ram.write(CustomCode::fnSetPacketOffset(game), Fn_setPacket);
		ram.write(CustomCode::fnSetFovOffset(game), Fn_setFov);
	}

	bool isApplied(const Game& game)
	{
		return game.ram().read<Mips_t>(game.offset().Fn_unknown2) == 0x27BDFFF0;
	}

	u32 fnSetPacketOffset(const Game& game)
	{
		return game.offset().Fn_unknown2;
	}

	u32 packetOffset(const Game& game)
	{
		return CustomCode::fnSetPacketOffset(game) + sizeof(CustomCode::Fn_SetPacket);
	}

	u32 fnSetFovOffset(const Game& game)
	{
		return CustomCode::packetOffset(game) + sizeof(CustomCode::Packet);
	}

	u32 fovOffset(const Game& game)
	{
		return CustomCode::fnSetFovOffset(game) + sizeof(CustomCode::Fn_SetFov);
	}
}
#pragma once

#include "Common/Camera3DControls.hpp"
#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Settings.hpp"
#include "Common/Types.hpp"
#include "Common/Vec3.hpp"

#include "Offset.hpp"

#include <array>

namespace PS2::ResidentEvil4
{
	using Packet = std::array<float, 52>;

	struct VersionDependency
	{
		u32 upShift;
	};

	class Loop final : public GameLoop
	{
	public:
		Loop(Ram&& ram, s32 version);
		~Loop();

		void draw() override;
		void update() override;
		bool isValid() override;
		void drawFreecam();
		void drawCamera();
		void drawOthers();
		void drawSettings();
		void drawBonus();
		void updateFreecam();
		void updateCamera();
		void updateOthers();
		void updateBonus();
		void enable(bool enable);
		void writeCamera();
		Packet createPacket() const;
		u32 fnSetPacketOffset() const;
		u32 packetOffset() const;
		u32 fnSetFovOffset() const;
		u32 fovOffset() const;
	private:
		Ram m_ram;
		s32 m_version;
		const Offset& m_offset;
		InputWrapper m_input;
		Camera3DControls m_controls;
		const VersionDependency m_dep;

		bool m_isEnabled{};

		u32 m_cameraPtr{};
		Vec3<float> m_position{};
		Vec3<float> m_rotation{};
		float m_fov{};

		bool m_isGamePaused{};
		bool m_isHudHidden{};
		bool m_isButtonEnabled{ true };
		bool m_isJoystickEnabled{ true };

		bool m_resetZRotation{ g_settings.gameSettings.resetRollRotation };
		bool m_pauseGame{ g_settings.gameSettings.pauseGame };
		bool m_hideHud{ g_settings.gameSettings.hideHud };
		bool m_disableButton{ g_settings.gameSettings.disableButton };
		bool m_disableJoystick{ g_settings.gameSettings.disableJoystick };
		bool m_resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool m_resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool m_resetFovSpeed{ g_settings.gameSettings.resetFovSpeed };

		bool m_noFog{};
		bool m_noGameOver{};
		bool m_noCollisions{};
		bool m_teleportToCamera{};
		bool m_unlockAll{};
	};
}
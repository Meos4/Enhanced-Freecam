#pragma once

#include "Common/Camera3DControls.hpp"
#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Mips.hpp"
#include "Common/Ram.hpp"
#include "Common/Settings.hpp"
#include "Common/Types.hpp"
#include "Common/Vec3.hpp"

#include "Game.hpp"
#include "Offset.hpp"

namespace PS2::DragonQuest8
{
	struct VersionDependency
	{
		u32 nearShift,
			ubShift,
			cgsShift;

		Mips_t
			spmInstr,
			spmInstr2,
			tInstr,
			dchInstr,
			dchInstr2,
			dbhInstr,
			dbhInstr2,
			tsInstr;
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
	private:
		Ram m_ram;
		s32 m_version;
		const Offset& m_offset;
		InputWrapper m_input;
		Camera3DControls m_controls;
		const VersionDependency m_dep;

		s32 m_state{ State::None };
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
		bool m_disableButton{ g_settings.gameSettings.disableButton };
		bool m_disableJoystick{ g_settings.gameSettings.disableJoystick };
		bool m_pauseGame{ g_settings.gameSettings.pauseGame };
		bool m_hideHud{ g_settings.gameSettings.hideHud };
		bool m_resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool m_resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool m_resetFovSpeed{ g_settings.gameSettings.resetFovSpeed };

		struct
		{
			float time;
			bool shouldChangeValue;
			bool isFrozen;
		} m_timer{};
		bool m_noNearClipTransparency{};
		bool m_noRandomEncounter{};
		bool m_giftItems{};
		bool m_statsMax{};
	};
}
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

#include <utility>

namespace PS2::DBZTenkaichi3
{
	struct VersionDependency
	{
		u32 cucShift;

		Mips_t 
			usInstr,
			dwlInstr;
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
		std::pair<u32, u32> vmPosPtr() const;
		u32 viewMatrixOffset() const;
	private:
		Ram m_ram;
		s32 m_version;
		const Offset& m_offset;
		InputWrapper m_input;
		Camera3DControls m_controls;
		const VersionDependency m_dep;

		s32 m_state{ State::None };
		bool m_isEnabled{};

		Vec3<float> m_position{};
		Vec3<float> m_rotation{};

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

		bool m_noBlur{};
		bool m_noAuras{};
		bool m_noNearTransparency{};
		bool m_noOnScreenEffects{};
		u8 m_shaders{ 128 };
		bool m_unlockAll{};
	};
}
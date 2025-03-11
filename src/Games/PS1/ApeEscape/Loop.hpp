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

namespace PS1::ApeEscape
{
	struct VersionDependency
	{
		u32 loopIndexShift,
			cShift,
			csShift,
			tpShift,
			avShift,
			ssShift,
			tpShift2,
			skrShift,
			sbShift,
			sbShift2,
			iglShift,
			csShift2,
			ssShift2,
			ssShift3,
			smShift,
			smShift2,
			spShift,
			spShift2,
			skrShift2,
			skrShift3,
			sbShift3,
			sbShift4,
			sbShift5,
			gmShift,
			gmShift2,
			ljShift,
			bStructShift2,
			ljYStructShift,
			rjXStructShift,
			rjYStructShift,
			ljShift2,
			bInstrShift,
			bShift,
			ljXStructShift,
			cShift2,
			tpShift3,
			tsShift,
			ssShift4,
			skrShift4,
			sbShift6;

		u16 bStructShift;

		Mips_t 
			skrInstr,
			skrInstr2,
			skrInstr3,
			sbInstr,
			sbInstr2,
			sbInstr3,
			sbInstr4,
			skrInstr4,
			skrInstr5,
			sbInstr5,
			gmInstr,
			tvInstr;
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
		u32 vmPtr(s32 state, bool read) const;
		void writeProjectionMatrix(s16 x, s16 y) const;
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

		Vec3<s16> m_position{};
		Vec3<float> m_positionFp{};
		Vec3<float> m_rotation{};
		float m_fov{ 1.f };

		bool m_isGamePaused{};
		bool m_isHudHidden{};
		bool m_isButtonEnabled{ true };
		bool m_isLJoystickEnabled{ true };
		bool m_isRJoystickEnabled{ true };

		bool m_resetZRotation{ g_settings.gameSettings.resetRollRotation };
		bool m_pauseGame{ g_settings.gameSettings.pauseGame };
		bool m_hideHud{ g_settings.gameSettings.hideHud };
		bool m_disableButton{ g_settings.gameSettings.disableButton };
		bool m_disableLJoystick{ g_settings.gameSettings.disableJoystick };
		bool m_disableRJoystick{ g_settings.gameSettings.disableJoystick };
		bool m_resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool m_resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool m_resetFovSpeed{ g_settings.gameSettings.resetFovSpeed };

		s32 m_titleScreenState{};
		bool m_noFog{};
		bool m_noTvEffect{};
		bool m_titleScreenNoTimer{};
		bool m_stageSelectSelectMenu{};
		bool m_stagePreviewNoTimer{};
	};
}
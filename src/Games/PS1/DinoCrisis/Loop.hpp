#pragma once

#include "Common/Camera3DControls.hpp"
#include "Common/GameInfo.hpp"
#include "Common/InputWrapper.hpp"
#include "Common/Ram.hpp"
#include "Common/Settings.hpp"
#include "Common/Types.hpp"
#include "Common/Vec3.hpp"

#include "Offset.hpp"

namespace PS1::DinoCrisis
{
	struct VersionDependency
	{
		u32 psShift;
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

		bool m_isEnabled{};

		Vec3<s16> m_position{};
		Vec3<float> m_positionFp{};
		Vec3<float> m_rotation{};
		float m_fov{ 1.f };

		bool m_isGamePaused{};
		bool m_isButtonEnabled{ true };

		bool m_pauseGame{ g_settings.gameSettings.pauseGame };
		bool m_disableButton{ g_settings.gameSettings.disableButton };
		bool m_resetMovementSpeed{ g_settings.gameSettings.resetMovementSpeed };
		bool m_resetRotationSpeed{ g_settings.gameSettings.resetRotationSpeed };
		bool m_resetFovSpeed{ g_settings.gameSettings.resetFovSpeed };

		bool m_noCutsceneBlackBar{};
		bool m_noGameOverFade{};
	};
}
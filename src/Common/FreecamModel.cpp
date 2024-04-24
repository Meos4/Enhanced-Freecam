#include "FreecamModel.hpp"

#include "InputWrapper.hpp"
#include "Settings.hpp"
#include "Ui.hpp"

#include <algorithm>
#include <format>

namespace FreecamModel
{
	void drawCameraName(const char* name)
	{
		ImGui::Text("Camera: %s", name);
	}

	void drawCameraName(std::span<const char* const> names, s32 current)
	{
		const bool isCurrentCameraInRange{ !(names.size() <= static_cast<std::size_t>(current)) };

		FreecamModel::drawCameraName(isCurrentCameraInRange ? names[current] : "None / Unknown");

		if (!isCurrentCameraInRange)
		{
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");

			std::string supportedCameras{ "Available:" };

			for (auto name : names)
			{
				supportedCameras += std::format("\n- {}", name);
			}

			Ui::hoveredTooltip(supportedCameras.c_str());
		}
	}

	bool drawEnable(bool* isEnabled)
	{
		return Ui::checkbox("Enable", isEnabled);
	}

	void drawSeparatorProperties()
	{
		Ui::separatorText("Properties");
	}

	void drawSeparatorOthers()
	{
		Ui::separatorText("Others");
	}

	void draw3DSettings()
	{
		auto* const s{ &g_settings };
		Ui::separatorText("Settings");
		const auto format{ Ui::arithmeticFormat<float>(s->freecamSettingsFloatDecimals) };
		Ui::slider(Ui::lol("Movement Speed"), &s->movementSpeed, format.c_str(), ImGuiSliderFlags_AlwaysClamp, s->movementSpeedMin, s->movementSpeedMax);
		Ui::slider(Ui::lol("Rotation Speed"), &s->rotationSpeed, format.c_str(), ImGuiSliderFlags_AlwaysClamp, s->rotationSpeedMin, s->rotationSpeedMax);
		Ui::slider(Ui::lol("Fov Speed"), &s->fovSpeed, format.c_str(), ImGuiSliderFlags_AlwaysClamp, s->fovSpeedMin, s->fovSpeedMax);
	}

	void draw3DSettingsNoFov()
	{
		auto* const s{ &g_settings };
		Ui::separatorText("Settings");
		const auto format{ Ui::arithmeticFormat<float>(s->freecamSettingsFloatDecimals) };
		Ui::slider(Ui::lol("Movement Speed"), &s->movementSpeed, format.c_str(), ImGuiSliderFlags_AlwaysClamp, s->movementSpeedMin, s->movementSpeedMax);
		Ui::slider(Ui::lol("Rotation Speed"), &s->rotationSpeed, format.c_str(), ImGuiSliderFlags_AlwaysClamp, s->rotationSpeedMin, s->rotationSpeedMax);
	}

	void increaseMovementSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->movementSpeed = std::clamp(s->movementSpeed *= s->movementSpeedScalar, s->movementSpeedMin, s->movementSpeedMax);
		}
	}

	void decreaseMovementSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->movementSpeed = std::clamp(s->movementSpeed /= s->movementSpeedScalar, s->movementSpeedMin, s->movementSpeedMax);
		}
	}

	void increaseRotationSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->rotationSpeed = std::clamp(s->rotationSpeed *= s->rotationSpeedScalar, s->rotationSpeedMin, s->rotationSpeedMax);
		}
	}

	void decreaseRotationSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->rotationSpeed = std::clamp(s->rotationSpeed /= s->rotationSpeedScalar, s->rotationSpeedMin, s->rotationSpeedMax);
		}
	}

	void increaseFovSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->fovSpeed = std::clamp(s->fovSpeed *= s->fovSpeedScalar, s->fovSpeedMin, s->fovSpeedMax);
		}
	}

	void decreaseFovSpeed(InputWrapper* input, s32 id)
	{
		if (input->isPressed(id))
		{
			auto* const s{ &g_settings };
			s->fovSpeed = std::clamp(s->fovSpeed /= s->fovSpeedScalar, s->fovSpeedMin, s->fovSpeedMax);
		}
	}

	void update3DSettings(InputWrapper* input, s32 im, s32 dm, s32 ir, s32 dr, s32 ifo, s32 df)
	{
		FreecamModel::increaseMovementSpeed(input, im);
		FreecamModel::decreaseMovementSpeed(input, dm);
		FreecamModel::increaseRotationSpeed(input, ir);
		FreecamModel::decreaseRotationSpeed(input, dr);
		FreecamModel::increaseFovSpeed(input, ifo);
		FreecamModel::decreaseFovSpeed(input, df);
	}

	void update3DSettingsNoFov(InputWrapper* input, s32 im, s32 dm, s32 ir, s32 dr)
	{
		FreecamModel::increaseMovementSpeed(input, im);
		FreecamModel::decreaseMovementSpeed(input, dm);
		FreecamModel::increaseRotationSpeed(input, ir);
		FreecamModel::decreaseRotationSpeed(input, dr);
	}

	float velocityInterpolation(float vel, float target, float dt)
	{
		static constexpr auto
			acceleration{ 9.f },
			deceleration{ acceleration / 3.f };

		if (target)
		{
			const auto delta{ target - vel };
			vel += delta * acceleration * dt;
			vel = std::clamp(vel, -1.f, 1.f);
		}
		else if (vel)
		{
			auto absVel{ std::abs(vel) };
			absVel -= deceleration * std::exp(absVel) * dt;

			if (absVel < 0.f)
			{
				absVel = 0.f;
			}

			vel = vel < 0.f ? -absVel : absVel;
		}

		return vel;
	}

	float movementVelocity(InputWrapper* input, s32 pos, s32 neg, float* velocity, float dt)
	{
		const auto vel{ input->sensitivity(pos) + -input->sensitivity(neg) };

		if (g_settings.smoothCamera)
		{
			*velocity = FreecamModel::velocityInterpolation(*velocity, vel, dt);
		}
		else
		{
			*velocity = vel;
		}

		return *velocity * g_settings.movementSpeed * g_settings.deltaTimeScalar * dt;
	}

	float rotationVelocity(InputWrapper* input, s32 pos, s32 neg, float dt)
	{
		const auto vel{ input->sensitivity(pos) + -input->sensitivity(neg) };
		return vel * g_settings.rotationSpeed * g_settings.deltaTimeScalar * dt;
	}

	float fovVelocity(InputWrapper* input, s32 pos, s32 neg, float dt)
	{
		const auto vel{ input->sensitivity(pos) + -input->sensitivity(neg) };
		return vel * g_settings.fovSpeed * g_settings.deltaTimeScalar * dt;
	}
}
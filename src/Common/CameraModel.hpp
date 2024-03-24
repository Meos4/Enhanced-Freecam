#pragma once

#include "Settings.hpp"
#include "TemplateTypes.hpp"
#include "Ui.hpp"
#include "Vec3.hpp"

#include <limits>

namespace CameraModel
{
	void drawRotation(float* x, float* y, float* z, bool readOnly);
	void drawFovDegrees(float* radians, bool readOnly);
	void rotateRoll(float* roll, float amount);
	void rotatePitch(float* pitch, float fovRadians, float amount);
	void rotatePitch(float* pitch, float amount);
	void rotateYaw(float* yaw, float fovRadians, float amount);
	void rotateYaw(float* yaw, float amount);
	void increaseFov(float* fov, float amount, float min, float max);
	void increaseFov(float* radians, float amount);

	template <Arithmetic T>
	void drawPosition(Vec3<T>* vec, float speed, bool readOnly,
		T min = std::numeric_limits<T>::lowest(), T max = std::numeric_limits<T>::max())
	{
		const auto flags{ (readOnly ? ImGuiSliderFlags_ReadOnly : ImGuiSliderFlags_None) | ImGuiSliderFlags_AlwaysClamp };
		Ui::dragVec3(Ui::lol("Position"), vec, speed, Ui::arithmeticFormat<T>(g_settings.positionFloatDecimals).c_str(), flags, min, max);
	}

	template <Arithmetic T>
	void drawFov(T* fov, bool readOnly, T min, T max)
	{
		const auto flags{ (readOnly ? ImGuiSliderFlags_ReadOnly : ImGuiSliderFlags_None) | ImGuiSliderFlags_AlwaysClamp };
		Ui::slider(Ui::lol("Fov"), fov, Ui::arithmeticFormat<T>(g_settings.fovFloatDecimals).c_str(), flags, min, max);
	}
}
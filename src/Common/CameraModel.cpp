#include "CameraModel.hpp"

#include "Math.hpp"

namespace CameraModel
{
	void drawRotation(Vec3<float>* rotation, bool readOnly)
	{
		const auto flags{ (readOnly ? ImGuiSliderFlags_ReadOnly : ImGuiSliderFlags_None) | ImGuiSliderFlags_AlwaysClamp };
		Ui::dragEulerAnglesDegrees(Ui::lol("Rotation"), rotation, g_settings.dragRadiansSpeed, g_settings.rotationFloatDecimals, flags);
	}

	void drawFovDegrees(float* radians, bool readOnly)
	{
		*radians = Math::toDegrees(*radians);
		CameraModel::drawFov(radians, readOnly, Math::toDegrees(g_settings.fovRadiansMin), Math::toDegrees(g_settings.fovRadiansMax));
		*radians = Math::toRadians(*radians);
	}

	void rotateRoll(float* roll, float amount)
	{
		*roll = Math::wrapPi(*roll + g_settings.fovRadiansScalar * amount);
	}

	void rotatePitch(float* pitch, float fovRadians, float amount)
	{
		*pitch = Math::wrapPi(*pitch + g_settings.fovRadiansScalar * fovRadians * amount);
	}

	void rotatePitch(float* pitch, float amount)
	{
		*pitch = Math::wrapPi(*pitch + g_settings.fovRadiansScalar * amount);
	}

	void rotateYaw(float* yaw, float fovRadians, float amount)
	{
		*yaw = Math::wrapPi(*yaw + g_settings.fovRadiansScalar * fovRadians * amount);
	}

	void rotateYaw(float* yaw, float amount)
	{
		*yaw = Math::wrapPi(*yaw + g_settings.fovRadiansScalar * amount);
	}

	void increaseFov(float* radians, float amount)
	{
		CameraModel::increaseFov(radians, amount, g_settings.fovRadiansMin, g_settings.fovRadiansMax);
	}

	void increaseFov(float* fov, float amount, float min, float max)
	{
		*fov = std::clamp(*fov + g_settings.fovRadiansScalar * *fov * amount, min, max);
	}
}
#include "Camera3DControls.hpp"

#include "FreecamModel.hpp"
#include "InputWrapper.hpp"
#include "Settings.hpp"

Camera3DControls::Camera3DControls(InputWrapper* input, const Input3D& input3D)
	: m_input(input), m_input3D(input3D)
{
}

float Camera3DControls::forwardVelocity(float dt)
{
	return FreecamModel::movementVelocity(m_input, m_input3D.forward, m_input3D.backward, &m_forwardVel, dt);
}

float Camera3DControls::rightVelocity(float dt)
{
	return FreecamModel::movementVelocity(m_input, m_input3D.right, m_input3D.left, &m_rightVel, dt);
}

float Camera3DControls::upVelocity(float dt)
{
	return FreecamModel::movementVelocity(m_input, m_input3D.up, m_input3D.down, &m_upVel, dt);
}

float Camera3DControls::rollVelocity(float dt)
{
	return FreecamModel::rotationVelocity(m_input, m_input3D.rotateRollPos, m_input3D.rotateRollNeg, dt);
}

float Camera3DControls::pitchVelocity(float dt)
{
	return FreecamModel::rotationVelocity(m_input, m_input3D.rotatePitchPos, m_input3D.rotatePitchNeg, dt);
}

float Camera3DControls::yawVelocity(float dt)
{
	return FreecamModel::rotationVelocity(m_input, m_input3D.rotateYawPos, m_input3D.rotateYawNeg, dt);
}

float Camera3DControls::fovVelocity(float dt)
{
	const auto vel{ m_input->sensitivity(m_input3D.fovPos) + -m_input->sensitivity(m_input3D.fovNeg) };
	return vel * g_settings.deltaTimeScalar * dt;
}

void Camera3DControls::resetVelocity()
{
	m_forwardVel = 0.f;
	m_rightVel = 0.f;
	m_upVel = 0.f;
}
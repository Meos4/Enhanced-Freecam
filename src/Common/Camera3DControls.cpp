#include "Camera3DControls.hpp"

#include "FreecamModel.hpp"
#include "InputWrapper.hpp"
#include "Renderer.hpp"
#include "Settings.hpp"

Camera3DControls::Camera3DControls(InputWrapper* input)
	: m_input(input)
{
}

float Camera3DControls::forwardVelocity(s32 pos, s32 neg)
{
	return FreecamModel::movementVelocity(m_input, pos, neg, &m_forwardVel, Renderer::deltaTime());
}

float Camera3DControls::rightVelocity(s32 pos, s32 neg)
{
	return FreecamModel::movementVelocity(m_input, pos, neg, &m_rightVel, Renderer::deltaTime());
}

float Camera3DControls::upVelocity(s32 pos, s32 neg)
{
	return FreecamModel::movementVelocity(m_input, pos, neg, &m_upVel, Renderer::deltaTime());
}

float Camera3DControls::rollVelocity(s32 pos, s32 neg)
{
	return FreecamModel::rotationVelocity(m_input, pos, neg, Renderer::deltaTime());
}

float Camera3DControls::pitchVelocity(s32 pos, s32 neg)
{
	return FreecamModel::rotationVelocity(m_input, pos, neg, Renderer::deltaTime());
}

float Camera3DControls::yawVelocity(s32 pos, s32 neg)
{
	return FreecamModel::rotationVelocity(m_input, pos, neg, Renderer::deltaTime());
}

float Camera3DControls::fovVelocity(s32 pos, s32 neg)
{
	const auto vel{ m_input->sensitivity(pos) + -m_input->sensitivity(neg) };
	return vel * g_settings.deltaTimeScalar * Renderer::deltaTime();
}

void Camera3DControls::resetVelocity()
{
	m_forwardVel = 0.f;
	m_rightVel = 0.f;
	m_upVel = 0.f;
}
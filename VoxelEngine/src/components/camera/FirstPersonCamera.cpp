#include "FirstPersonCamera.h"

namespace VoxelEngine::components::camera
{
	FirstPersonCamera::FirstPersonCamera(const glm::vec3& position, const glm::vec3& up, const float& yaw, const float& pitch)
		: Camera(position),
		m_up(up),
		m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_yaw(yaw),
		m_pitch(pitch)
	{

	}

	void FirstPersonCamera::processKeyboard(const CameraMovement& direction, const float& deltaTime)
	{
		const float velocity = m_movementSpeed * deltaTime;
		m_lerpT = std::clamp(m_lerpT + deltaTime, 0.0f, 1.0f);

		if (m_lerpT > 1.0f)
			m_lerpT = 1.0f;

		glm::vec3 cameraDir;

		switch (direction)
		{
		case CameraMovement::Forward: 
			cameraDir = m_front;
			break;
		case CameraMovement::Backward:
			cameraDir = -m_front;
			break;
		case CameraMovement::Left:
			cameraDir = -glm::normalize(glm::cross(m_front, m_up));
			break;
		case CameraMovement::Right:
			cameraDir = glm::normalize(glm::cross(m_front, m_up));
			break;
		}

		m_position = glm::mix(m_position, m_position + cameraDir * velocity, m_lerpT);
	}
	void FirstPersonCamera::processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch)
	{
		m_pitch += yOffset * m_mouseSensitivity;
		m_yaw += xOffset * m_mouseSensitivity;

		if (constrainPitch) 
		{
			m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
		}

		glm::vec3 front;
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_front = glm::normalize(front);
	}
}
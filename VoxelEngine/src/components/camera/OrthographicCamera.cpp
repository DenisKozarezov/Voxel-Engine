#include "OrthographicCamera.h"

namespace VoxelEngine::components::camera
{
	OrthographicCamera::OrthographicCamera(
		const glm::vec3& position,
		const glm::vec3& up)
		: Camera(position),
		m_up(up),
		m_front(glm::vec3(0.0f, 0.0f, -1.0f))
	{

	}

	void OrthographicCamera::processKeyboard(const CameraMovement& direction, const float& deltaTime)
	{
		const float velocity = m_movementSpeed * deltaTime;
		m_lerpT = std::clamp(m_lerpT + deltaTime, 0.0f, 1.0f);

		if (m_lerpT > 1.0f)
			m_lerpT = 1.0f;

		glm::vec3 cameraDir;

		switch (direction)
		{
		case CameraMovement::Forward:
			cameraDir.x = -sin(glm::radians(defaultPitch));
			cameraDir.y = cos(glm::radians(defaultPitch));
			break;
		case CameraMovement::Backward:
			cameraDir.x = sin(glm::radians(defaultPitch));
			cameraDir.y = -cos(glm::radians(defaultPitch));
			break;
		case CameraMovement::Left:
			cameraDir.x = -cos(glm::radians(defaultPitch));
			cameraDir.y = -sin(glm::radians(defaultPitch));
			break;
		case CameraMovement::Right:
			cameraDir.x = cos(glm::radians(defaultPitch));
			cameraDir.y = sin(glm::radians(defaultPitch));
			break;
		}

		m_position = glm::mix(m_position, m_position + cameraDir * velocity, m_lerpT);
	}
	void OrthographicCamera::processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch)
	{
		
	}
}
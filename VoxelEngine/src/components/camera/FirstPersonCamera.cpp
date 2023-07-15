#include "FirstPersonCamera.h"

namespace VoxelEngine::components::camera
{
	FirstPersonCamera::FirstPersonCamera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
		: Camera(position),
		_movementSpeed(defaultSpeed), 
		_mouseSensitivity(defaultSensitivity), 
		_zoom(defaultZoom),
		_up(up),
		_front(glm::vec3(0.0f, 0.0f, -1.0f)),
		_yaw(yaw),
		_pitch(pitch)
	{

	}
	void FirstPersonCamera::processKeyboard(const CameraMovement& direction, const float& deltaTime)
	{
		const float velocity = _movementSpeed * deltaTime;
		switch (direction)
		{
		case CameraMovement::Forward: 
			_position += _front * velocity; 
			break;
		case CameraMovement::Backward:
			_position -= _front * velocity;
			break;
		case CameraMovement::Left:
			_position -= glm::normalize(glm::cross(_front, _up)) * velocity;
			break;
		case CameraMovement::Right:
			_position += glm::normalize(glm::cross(_front, _up)) * velocity;
			break;
		}
	}
	void FirstPersonCamera::processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch)
	{
		_pitch += yOffset * _mouseSensitivity;
		_yaw += xOffset * _mouseSensitivity;

		if (constrainPitch) 
		{
			_pitch = std::clamp(_pitch, -89.0f, 89.0f);
		}

		glm::vec3 front;
		front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		front.y = sin(glm::radians(_pitch));
		front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
		_front = glm::normalize(front);
	}
}
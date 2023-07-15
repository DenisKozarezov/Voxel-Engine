#pragma once
#include <glm/ext/matrix_transform.hpp>
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	constexpr static float defaultYaw = -90.0f;
	constexpr static float defaultPitch = 0.0f;
	constexpr static float defaultSpeed = 20.0f;
	constexpr static float defaultSensitivity = 0.05f;
	constexpr static float defaultZoom = 45.0f;

	class FirstPersonCamera
	{
	private:
		glm::vec3 _position;
		glm::vec3 _front;
		glm::vec3 _up;
		glm::vec3 _right;
		glm::vec3 _worldUp;
		float _yaw, _pitch;
		float _movementSpeed;
		float _mouseSensitivity;
		float _zoom;
	public:
		FirstPersonCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = defaultYaw,
			float pitch = defaultPitch);
		FirstPersonCamera(FirstPersonCamera const&) noexcept = delete;
		FirstPersonCamera(FirstPersonCamera&&) noexcept = delete;
		FirstPersonCamera& operator= (FirstPersonCamera const& rhs) noexcept = delete;
		FirstPersonCamera& operator= (FirstPersonCamera&& rhs) noexcept = delete;

		inline const glm::mat4& viewMatrix() const { return glm::lookAt(_position, _position + _front, _up); }
	
		void processKeyboard(const CameraMovement& direction, const float& deltaTime);
		void processMouse(const float& xOffset, const float& yOffset);
		void updateCameraVectors();
	};
}
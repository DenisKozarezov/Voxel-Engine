#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <algorithm>
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	constexpr static float defaultYaw = -90.0f;
	constexpr static float defaultPitch = 0.0f;
	constexpr static float defaultSpeed = 20.0f;
	constexpr static float defaultSensitivity = 0.15f;
	constexpr static float defaultZoom = 45.0f;

	class FirstPersonCamera : public Camera
	{
	private:
		glm::vec3 _front;
		glm::vec3 _up;
		float _yaw, _pitch;
		float _movementSpeed;
		float _mouseSensitivity;
		float _zoom;
	public:
		FirstPersonCamera() noexcept = delete;
		FirstPersonCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = defaultYaw,
			float pitch = defaultPitch);
		~FirstPersonCamera() noexcept = default;

		inline const glm::mat4& viewMatrix() const override { return glm::lookAt(_position, _position + _front, _up); }
	
		void processKeyboard(const CameraMovement& direction, const float& deltaTime) override;
		void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) override;
	};
}
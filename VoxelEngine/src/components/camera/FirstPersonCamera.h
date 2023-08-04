#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <algorithm>
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	constexpr static float defaultYaw = -90.0f;
	constexpr static float defaultPitch = 0.0f;
	constexpr static float defaultSpeed = 10.0f;
	constexpr static float defaultSensitivity = 0.15f;
	constexpr static float defaultZoom = 45.0f;

	static constexpr float FOV = 45.0f;

	static constexpr float nearClip = 0.1f;
	static constexpr float farClip = 200.0f;

	class FirstPersonCamera : public Camera
	{
	private:
		glm::vec3 _front;
		glm::vec3 _up;
		float _yaw, _pitch;
		float _movementSpeed;
		float _mouseSensitivity;
		float _zoom;
		float lerpT = 0;
	public:
		FirstPersonCamera() noexcept = delete;
		FirstPersonCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			float yaw = defaultYaw,
			float pitch = defaultPitch);
		~FirstPersonCamera() noexcept = default;

		inline const glm::mat4 viewMatrix() const override { return glm::lookAt(_position, _position + _front, _up); }
		inline const glm::mat4 projectionMatrix(const float& aspectRatio) const override 
		{ 
			glm::mat4 projection = glm::perspective(glm::radians(FOV), aspectRatio, nearClip, farClip);
			projection[1][1] *= -1;
			return projection;
		};

		void processKeyboard(const CameraMovement& direction, const float& deltaTime) override;
		void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) override;
	};
}
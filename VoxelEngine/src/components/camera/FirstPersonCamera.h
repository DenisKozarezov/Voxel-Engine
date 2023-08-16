#pragma once
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	class FirstPersonCamera : public Camera
	{
	private:
		constexpr static float defaultYaw = -90.0f;
		constexpr static float defaultPitch = 0.0f;
		constexpr static float FOV = 45.0f;
		constexpr static float nearClip = 0.1f;
		constexpr static float farClip = 200.0f;

		glm::vec3 m_front;
		glm::vec3 m_up;
		float m_yaw, m_pitch;
		float m_lerpT = 0.0f;
	public:
		FirstPersonCamera() noexcept = delete;
		FirstPersonCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			const float& yaw = defaultYaw,
			const float& pitch = defaultPitch);
		~FirstPersonCamera() noexcept = default;

		inline const glm::mat4 viewMatrix() const override { return glm::lookAt(m_position, m_position + m_front, m_up); }
		inline const glm::mat4 projectionMatrix() const override 
		{ 
			glm::mat4 projection = glm::perspective(glm::radians(FOV), m_aspectRatio, nearClip, farClip);
			projection[1][1] *= -1;
			return projection;
		};

		void processKeyboard(const CameraMovement& direction, const float& deltaTime) override;
		void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) override;
	};
}
#pragma once
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	class OrthographicCamera : public Camera
	{
	private:
		constexpr static float defaultYaw = 0.0f;
		constexpr static float defaultPitch = -90.0f;

		glm::mat4 m_projectionMatrix;
		glm::vec3 m_front;
		glm::vec3 m_up;
		float m_lerpT = 0.0f;
		float m_zoomLevel = 1.0f;
	public:
		OrthographicCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));
		~OrthographicCamera() override = default;
		
		FORCE_INLINE glm::mat4 viewMatrix() const override 
		{ 
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(defaultPitch), glm::vec3(0, 1, 0));
			return glm::inverse(transform);
		}
		FORCE_INLINE glm::mat4 projectionMatrix() const override
		{ 
			return glm::ortho(-m_aspectRatio, m_aspectRatio, -1.0f, 1.0f, m_nearClip, m_farClip);
		}

		void processKeyboard(const CameraMovement& direction, const float& deltaTime) override;
		void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) override;
	};
}
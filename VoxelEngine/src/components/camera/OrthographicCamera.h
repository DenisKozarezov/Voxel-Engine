#pragma once
#include "Camera.h"

namespace VoxelEngine::components::camera
{
	class OrthographicCamera : public Camera
	{
	private:
		constexpr static float nearClip = 0.1f;
		constexpr static float farClip = 100.0f;

		glm::mat4 m_projectionMatrix;
		float m_rotation = -90.0f;
		glm::vec3 m_front;
		glm::vec3 m_up;
		float m_lerpT = 0.0f;
		float m_zoomLevel = 1.0f;
	public:
		OrthographicCamera(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f));

		inline void setBounds(
			const float& left,
			const float& right,
			const float& bottom,
			const float& top)
		{
			m_projectionMatrix = glm::ortho(left, right, bottom, top, nearClip, farClip);
			m_projectionMatrix[1][1] *= -1;
		}
		inline void setRotation(const float& rotation) { m_rotation = rotation; }
		inline void setAspectRatio(const float& aspectRatio) override;

		inline const glm::mat4 viewMatrix() const override { return glm::lookAt(m_position, m_position + glm::vec3(0.0f, -1.0f, 0.0f), m_up); }
		inline const glm::mat4 projectionMatrix() const override { return m_projectionMatrix; }

		void processKeyboard(const CameraMovement& direction, const float& deltaTime) override;
		void processMouse(const float& xOffset, const float& yOffset, const bool& constrainPitch = true) override;
	};
}
#pragma once
#include "Camera.h"
#include <InputCore/events/MouseEvent.h>
#include <InputCore/InputSystem.h>
#include <Core/Internal/Timestep.h>

namespace VoxelEngine::components::camera
{
	class EditorCameraController
	{
	private:
		ProjectionType m_projectionType;
		Camera* m_currentCamera = nullptr;
		glm::vec3 m_lastPosition;
		glm::vec2 m_lastMousePos;
		input::MouseDraggingState m_mouseState;

		void onKeyboardUpdate(const Timestep& ts);
		void onMouseUpdate();
	public:
		EditorCameraController() noexcept = default;
		EditorCameraController(
			const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
			const ProjectionType& mode = ProjectionType::Perspective);

		INLINE operator Camera&() { return *m_currentCamera; };

		INLINE const glm::vec3 getPosition() const { return m_currentCamera->getPosition(); }
		INLINE const glm::mat4 viewMatrix() const { return m_currentCamera->viewMatrix(); }
		INLINE const glm::mat4 projectionMatrix() const { return m_currentCamera->projectionMatrix(); }
		INLINE const ProjectionType& projectionType() const & noexcept { return m_projectionType; }

		void setAspectRatio(const float& aspectRatio);
		void setClips(const float& nearClip, const float& farClip);
		void setSpeed(const float& speed);
		void switchMode(const ProjectionType& mode);
		void setMouseDragging(const bool& isDragging);

		void update(const Timestep& ts);

		~EditorCameraController();
	};

	INLINE void EditorCameraController::setAspectRatio(const float& aspectRatio)
	{
		m_currentCamera->setAspectRatio(aspectRatio);
	}
	INLINE void EditorCameraController::setClips(const float& nearClip, const float& farClip)
	{
		m_currentCamera->setClips(nearClip, farClip);
	}
	INLINE void EditorCameraController::setSpeed(const float& speed)
	{
		m_currentCamera->setSpeed(speed);
	}
}
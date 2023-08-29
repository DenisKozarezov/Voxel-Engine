#pragma once
#include "Camera.h"
#include <core/input/events/MouseEvent.h>
#include <core/input/InputSystem.h>
#include <core/Timestep.h>

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

		inline operator Camera&() { return *m_currentCamera; };

		inline const glm::vec3 getPosition() const { return m_currentCamera->getPosition(); }
		inline const glm::mat4 viewMatrix() const { return m_currentCamera->viewMatrix(); }
		inline const glm::mat4 projectionMatrix() const { return m_currentCamera->projectionMatrix(); }
		inline const ProjectionType& projectionType() const & noexcept { return m_projectionType; }

		void switchMode(const ProjectionType& mode);
		inline void setAspectRatio(const float& aspectRatio) { m_currentCamera->setAspectRatio(aspectRatio); }
		inline void setClips(const float& nearClip, const float& farClip) { m_currentCamera->setClips(nearClip, farClip); }
		void setMouseDragging(const bool& isDragging);

		void update(const Timestep& ts);

		~EditorCameraController();
	};
}
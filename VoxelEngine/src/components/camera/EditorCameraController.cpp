#include "EditorCameraController.h"
#include "FirstPersonCamera.h"
#include "OrthographicCamera.h"
#include <core/Log.h>

namespace VoxelEngine::components::camera
{
	void EditorCameraController::onKeyboardUpdate(const Timestep& ts)
	{
		if (input::InputSystem::isKeyPressed(input::W))
		{
			m_currentCamera->processKeyboard(components::camera::CameraMovement::Forward, ts);
		}
		if (input::InputSystem::isKeyPressed(input::S))
		{
			m_currentCamera->processKeyboard(components::camera::CameraMovement::Backward, ts);
		}
		if (input::InputSystem::isKeyPressed(input::A))
		{
			m_currentCamera->processKeyboard(components::camera::CameraMovement::Left, ts);
		}
		if (input::InputSystem::isKeyPressed(input::D))
		{
			m_currentCamera->processKeyboard(components::camera::CameraMovement::Right, ts);
		}
	}
	void EditorCameraController::onMouseUpdate()
	{
		glm::vec2 mousePos = input::InputSystem::getMousePosition();

		if (m_mouseState == input::MouseDraggingState::DragBegin)
		{
			m_mouseState = input::MouseDraggingState::Dragging;
		}
		
		if (m_mouseState == input::MouseDraggingState::Dragging)
		{
			m_currentCamera->processMouse(mousePos.x - m_lastMousePos.x, m_lastMousePos.y - mousePos.y);
			m_lastMousePos = mousePos;
		}
	}
	EditorCameraController::EditorCameraController(const glm::vec3& position, const ProjectionType& mode)
		: m_lastPosition(position)
	{
		switchMode(mode);
	}

	void EditorCameraController::switchMode(const ProjectionType& mode)
	{
		m_projectionType = mode;

		if (m_currentCamera)
		{
			m_lastPosition = m_currentCamera->getPosition();
			delete m_currentCamera;
		}

		switch (mode)
		{
		case ProjectionType::Perspective:	
			m_currentCamera = new FirstPersonCamera(m_lastPosition);
			break;
		case ProjectionType::Orthographic:
			m_currentCamera = new OrthographicCamera(m_lastPosition);
			break;
		case ProjectionType::Isometric:
			VOXEL_WARN("Isometric camera mode is not supported yet!");
			break;
		}
	}
	void EditorCameraController::setMouseDragging(const bool& isDragging)
	{
		if (isDragging)
		{
			glm::vec2 mousePos = input::InputSystem::getMousePosition();
			m_lastMousePos = mousePos;
		}

		m_mouseState = isDragging ? input::MouseDraggingState::DragBegin : input::MouseDraggingState::None;
	}
	void EditorCameraController::update(const Timestep& ts)
	{
		onKeyboardUpdate(ts);

		if (m_mouseState != input::MouseDraggingState::None)
			onMouseUpdate();
	}
	EditorCameraController::~EditorCameraController()
	{
		delete m_currentCamera;
	}
}

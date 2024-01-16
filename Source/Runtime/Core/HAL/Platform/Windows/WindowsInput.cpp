#include <InputCore/InputSystem.h>
#include <Core/Internal/Application.h>
#include <GLFW/glfw3.h>

namespace VoxelEngine::input
{
	bool InputSystem::isKeyPressed(const KeyCode& key)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance()->getWindow()->getNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool InputSystem::isMouseButtonPressed(const MouseCode& button)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance()->getWindow()->getNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool InputSystem::isKeyReleased(const KeyCode& key)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance()->getWindow()->getNativeWindow());
		int state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_RELEASE;
	}

	bool InputSystem::isMouseButtonReleased(const MouseCode& button)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance()->getWindow()->getNativeWindow());
		int state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_RELEASE;
	}

	glm::vec2 InputSystem::getMousePosition()
	{
		double xpos, ypos;
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance()->getWindow()->getNativeWindow());
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}
}

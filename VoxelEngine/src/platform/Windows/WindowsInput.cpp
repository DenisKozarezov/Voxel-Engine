#include <core/input/InputSystem.h>
#include <core/Application.h>
#include <GLFW/glfw3.h>

namespace VoxelEngine::input
{
	const bool InputSystem::isKeyPressed(const KeyCode& key)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance().getWindow()->getNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}
	const bool InputSystem::isMouseButtonPressed(const MouseCode& button)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance().getWindow()->getNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}
	const bool InputSystem::isKeyReleased(const KeyCode& key)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance().getWindow()->getNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_RELEASE;
	}
	const bool InputSystem::isMouseButtonReleased(const MouseCode& button)
	{
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance().getWindow()->getNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_RELEASE;
	}
	const glm::vec2 InputSystem::getMousePosition()
	{
		double xpos, ypos;
		GLFWwindow* window = (GLFWwindow*)(Application::getInstance().getWindow()->getNativeWindow());
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}
	INLINE const float InputSystem::getMouseX()
	{
		return getMousePosition().x;
	}
	INLINE const float InputSystem::getMouseY()
	{
		return getMousePosition().y;
	}

}
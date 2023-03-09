#include "Application.h"

namespace core
{
	GLFWwindow* const Application::createWindow(const int& width, const int& height, const std::string& title) const noexcept
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	}

	const void Application::init()
	{
		_window = createWindow(800, 600, "Vulkan APP");
		_renderer.setGLFWwindow(_window);

		try
		{
			_renderer.init();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what();
		}
	}
	const void Application::run()
	{
		_running = true;

		while (!glfwWindowShouldClose(_window))
		{
			glfwPollEvents();
			_renderer.renderFrame();
		}
		_renderer.deviceWaitIdle();
	}
	const void Application::shutdown()
	{
		_running = false;

		_renderer.cleanup();

		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	Application::~Application()
	{
		if (_running)
		{
			shutdown();
		}
	}
}
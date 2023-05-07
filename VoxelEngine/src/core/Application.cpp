#include "Application.h"
#include "Log.h"
#include "Assert.h"

namespace VoxelEngine
{
	Application* Application::_instance = 0;

	GLFWwindow* const Application::createWindow(const int& width, const int& height, const string& title) const noexcept
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	}

	Application::Application()
	{
		VOXEL_CORE_ASSERT(!_instance, "Application already exists!")

		_instance = this;

		_window = createWindow(800, 600, "Voxel Editor");
	}

	inline std::unique_ptr<Application>& Application::getInstance()
	{
		static std::unique_ptr<Application> ptr = std::unique_ptr<Application>(_instance);
		return ptr;
	}

	const void Application::init()
	{
		_renderer.setGLFWwindow(_window);

		try
		{
			_renderer.init();
		}
		catch (const std::exception& e)
		{
			VOXEL_CORE_CRITICAL(e.what())
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

	void Application::onEvent(const input::Event& e)
	{
	}
	void Application::onWindowClosed(const input::WindowCloseEvent& e)
	{
		shutdown();
	}
	void Application::onWindowResized(const input::WindowResizeEvent& e)
	{
	}
}
#include "Application.h"
#include "input/events/EventBus.h"

namespace VoxelEngine
{
	Application* Application::_instance = 0;

	Application::Application(const ApplicationSpecification& spec) : _specification(spec)
	{
		VOXEL_CORE_ASSERT(!_instance, "Application already exists!")

		_instance = this;

		VOXEL_CORE_WARN("Application Name: " + spec.ApplicationName)
		VOXEL_CORE_WARN("Version: " + spec.Version)
		VOXEL_CORE_WARN("Working Directory: " + spec.WorkingDirectory)
		VOXEL_CORE_WARN("Command Line Args: " + spec.CommandLineArgs.toString())
		
		string name = spec.ApplicationName + " " + spec.Version + " (" + spec.GraphicsAPI + ")";
		_window = Window::Create({ name, 1920, 1080 });
		_window->setEventCallback(BIND_CALLBACK(onEvent));
	}

	inline UniqueRef<Application>& Application::getInstance()
	{
		static UniqueRef<Application> ptr = UniqueRef<Application>(_instance);
		return ptr;
	}

	const void Application::init()
	{
	//	try
	//	{
	//		_renderer.init();
	//		_renderer.setGLFWwindow(_window);
	//	}
	//	catch (const std::exception& e)
	//	{
	//		VOXEL_CORE_CRITICAL(e.what())
	//	}
	}
	const void Application::run()
	{
		VOXEL_CORE_WARN("Running Voxel Engine...")

		_running = true;

		while (_running)
		{
			_window->onUpdate();
			//_renderer.renderFrame();
		}
		//_renderer.deviceWaitIdle();
	}
	const void Application::shutdown()
	{
		_running = false;
	}

	void Application::onEvent(input::Event& e)
	{
		input::EventBus dispatcher(e);
		dispatcher.Fire<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		dispatcher.Fire<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
	}
	bool Application::onWindowClose(const input::WindowCloseEvent& e)
	{
		shutdown();
		return true;
	}
	bool Application::onWindowResize(const input::WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			return false;
		}
		return false;
	}
}
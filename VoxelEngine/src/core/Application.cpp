#include "Application.h"
#include "input/events/EventBus.h"
#include "Timestep.h"
#include "renderer/Renderer.h"
#include "renderer/ImGuiLayer.h"

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

		std::stringstream name;
		name << spec.ApplicationName << " " << spec.Version << " (" << spec.GraphicsAPI << ")";
		_window = Window::Create({ name.str(), 1920, 1080 });
		_window->setEventCallback(BIND_CALLBACK(onEvent));
	}

	const SharedRef<Application> Application::getInstance()
	{
		return SharedRef<Application>(_instance);
	}

	void Application::pushLayer(renderer::Layer* layer)
	{
		_layerStack.pushLayer(layer);
		layer->onAttach();
	}
	void Application::pushOverlay(renderer::Layer* layer)
	{
		_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	const void Application::init()
	{
		try
		{
			pushOverlay(new renderer::ImGuiLayer());
			renderer::Renderer::init(*_window.get());
		}
		catch (const std::exception& e)
		{
			VOXEL_CORE_CRITICAL(e.what())
			VOXEL_DEBUGBREAK()
		}
	}
	const void Application::run()
	{
		VOXEL_CORE_WARN("Running Voxel Engine...")

		_running = true;

		while (_running)
		{
			const float time = renderer::Renderer::getTime();
			const Timestep deltaTime = time - _lastFrameTime;
			_lastFrameTime = time;

			if (!_minimized)
			{
				_layerStack.onUpdate(time);
				renderer::Renderer::beginFrame();
				_layerStack.onImGuiRender();
				renderer::Renderer::endFrame();
			}
			_window->onUpdate();
		}
		renderer::Renderer::deviceWaitIdle();
	}
	const void Application::shutdown()
	{
		_layerStack.detach();
		renderer::Renderer::cleanup();
	}

	void Application::onEvent(input::Event& e)
	{
		input::EventBus dispatcher(e);
		dispatcher.Fire<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		dispatcher.Fire<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
	}
	bool Application::onWindowClose(const input::WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}
	bool Application::onWindowResize(const input::WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			_minimized = true;
			return false;
		}
		_minimized = false;
		return false;
	}
}
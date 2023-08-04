#include "Application.h"
#include "renderer/Renderer.h"
#include "imgui/ImGuiLayer.h"

namespace VoxelEngine
{
	Application* Application::_instance = 0;
	renderer::ImGuiLayer* imguiLayer = nullptr;

	Application::Application(const ApplicationSpecification& spec) : _specification(spec)
	{
		VOXEL_CORE_ASSERT(!_instance, "Application already exists!")

		_instance = this;

		VOXEL_CORE_WARN("Application Name: {0}", spec.ApplicationName)
		VOXEL_CORE_WARN("Version: {0}", spec.Version)
		VOXEL_CORE_WARN("Working Directory: {0}", spec.WorkingDirectory)
		VOXEL_CORE_WARN("Command Line Args: {0}", spec.CommandLineArgs.toString())

		setupInputCallbacks();

		std::stringstream name;
		name << spec.ApplicationName << " " << spec.Version << " (" << spec.GraphicsAPI << ")";
		_window = Window::Create({ name.str(), 1920, 1080 });
		_window->setEventCallback(BIND_CALLBACK(onEvent));
		_window->setMaximized(spec.Maximized);
	}

	Application& Application::getInstance()
	{
		return *_instance;
	}
	const float& Application::getDeltaTime() const
	{
		return _frameTimer;
	}
	const uint32& Application::getFPS() const
	{
		return _lastFPS;
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

	void Application::init()
	{
		try
		{
			VOXEL_CORE_WARN("Application initialization.")
			imguiLayer = new renderer::ImGuiLayer();
			pushOverlay(imguiLayer);
			renderer::Renderer::init(*_window.get());
		}
		catch (const std::exception& e)
		{
			VOXEL_CORE_CRITICAL(e.what())
			VOXEL_DEBUGBREAK()
		}
	}
	void Application::run()
	{
		VOXEL_CORE_WARN("Running {0}...", _specification.ApplicationName)

		_running = true;

		while (_running)
		{
			nextFrame();
			_window->onUpdate();
		}
		renderer::Renderer::deviceWaitIdle();
	}
	void Application::shutdown()
	{
		_layerStack.detach();
		renderer::Renderer::cleanup();
	}

	void Application::setupInputCallbacks()
	{
		_dispatcher.registerEvent<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		_dispatcher.registerEvent<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
	}
	void Application::nextFrame()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		if (!_minimized)
		{
			renderer::Renderer::beginFrame();
			imguiLayer->beginFrame();

			_layerStack.onImGuiRender();

			imguiLayer->endFrame();
			renderer::Renderer::endFrame();

			_frameCounter++;

			auto tEnd = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			_frameTimer = (float)tDiff / 1000.0f;
			
			_layerStack.onUpdate(_frameTimer);

			_accumulator += _frameTimer;
			while (_accumulator >= fixedDeltaTime)
			{
				_layerStack.onFixedUpdate(fixedDeltaTime);
				_accumulator -= fixedDeltaTime;
			}

			float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count());
			if (fpsTimer > 1000.0f)
			{
				_lastFPS = static_cast<uint32>((float)_frameCounter * (1000.0f / fpsTimer));
				_frameCounter = 0;
				lastTimestamp = tEnd;
			}
			tPrevEnd = tEnd;
		}
	}
	void Application::onEvent(input::Event& e)
	{
		_dispatcher.dispatchEvent(e, std::launch::async);
		_layerStack.onEvent(e);
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
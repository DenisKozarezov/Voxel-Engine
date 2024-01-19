#include "Application.h"
#include <InputCore/events/EventDispatcher.h>
#include <Renderer/Renderer.h>

namespace VoxelEngine
{
	Application* Application::s_instance = 0;
	renderer::ImGuiLayer* Application::s_imguiLayer = nullptr;

	Application::Application(const ApplicationSpecification& spec) : m_specification(spec)
	{
		VOXEL_CORE_ASSERT(!s_instance, "Application already exists!");

		s_instance = this;

		RUNTIME_WARN("Application Name: {0}", spec.ApplicationName);
		RUNTIME_WARN("Version: {0}", spec.Version);
		RUNTIME_WARN("Working Directory: {0}", spec.WorkingDirectory);
		RUNTIME_WARN("Command Line Args: {0}", spec.CommandLineArgs.str());

		setupInputCallbacks();

		std::stringstream name;
		string graphicsSpec = graphicsSpecString(renderer::g_graphicsSpec);

		RUNTIME_WARN("Graphics Renderer API: {0}", graphicsSpec);

		name << spec.ApplicationName << " " << spec.Version << " (" << graphicsSpec << ")";
		m_window = Window::Create({ name.str(), 1920, 1080 });
		m_window->setEventCallback(BIND_CALLBACK(onEvent));
		m_window->setMaximized(spec.Maximized);

		init();
	}

	void Application::pushLayer(renderer::Layer* layer)
	{
		m_layerStack.pushLayer(layer);
		layer->onAttach();
	}
	void Application::pushOverlay(renderer::Layer* layer)
	{
		m_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	void Application::init()
	{
		try
		{
			RUNTIME_WARN("Application initialization.");
			s_imguiLayer = new renderer::ImGuiLayer();
			pushOverlay(s_imguiLayer);
			renderer::Renderer::init(*m_window.get());
		}
		catch (const std::exception& e)
		{
			RUNTIME_CRITICAL(e.what());
			PLATFORM_BREAK();
		}
	}
	void Application::run()
	{
		RUNTIME_WARN("Running {0}...", m_specification.ApplicationName);

		m_running = true;

		while (m_running)
		{
			nextFrame();
			m_window->onUpdate();
		}
		renderer::Renderer::deviceWaitIdle();
	}
	void Application::shutdown()
	{
		m_layerStack.detach();
		renderer::Renderer::shutdown();
	}

	Application::~Application()
	{
		m_eventDispatcher.unregisterEvent<input::WindowCloseEvent>();
		m_eventDispatcher.unregisterEvent<input::WindowResizeEvent>();
	}

	void Application::setupInputCallbacks()
	{
		m_eventDispatcher.registerEvent<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		m_eventDispatcher.registerEvent<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
	}
	void Application::nextFrame()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		if (!m_minimized)
		{
			s_imguiLayer->preRender();
			m_layerStack.onImGuiRender();
			s_imguiLayer->postRender();

			m_layerStack.onUpdate(m_frameTimer);

			m_accumulator += m_frameTimer;
			while (m_accumulator >= fixedDeltaTime)
			{
				m_layerStack.onFixedUpdate(fixedDeltaTime);
				m_accumulator -= fixedDeltaTime;
			}

			m_frameCounter++;

			auto tEnd = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			m_frameTimer = tDiff / 1000.0;

			calculateFramerate(tEnd);
		}
	}
	void Application::calculateFramerate(const std::chrono::steady_clock::time_point& tEnd)
	{
		double fpsTimer = std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count();
		if (fpsTimer > 1000.0)
		{
			m_lastFPS = static_cast<uint32>(m_frameCounter * (1000.0 / fpsTimer));
			m_frameCounter = 0;
			lastTimestamp = tEnd;
		}
		tPrevEnd = tEnd;
	}
	void Application::onEvent(input::Event& e)
	{
		m_eventDispatcher.dispatchEvent(e, std::launch::async);
		m_layerStack.onEvent(e);
	}
	bool Application::onWindowClose(const input::WindowCloseEvent& e)
	{
		m_running = false;
		return true;
	}
	bool Application::onWindowResize(const input::WindowResizeEvent& e)
	{
		uint16 width = e.getWidth();
		uint16 height = e.getHeight();

		renderer::Renderer::resize(width, height);

		if (width == 0 || height == 0)
		{
			m_minimized = true;
			return false;
		}
		m_minimized = false;
		return false;
	}
}
#include "Application.h"
#include "imgui/ImGuiLayer.h"

namespace VoxelEngine
{
	Application* Application::s_instance = 0;
	renderer::ImGuiLayer* g_imguiLayer = nullptr;

	Application::Application(const ApplicationSpecification& spec) : m_specification(spec)
	{
		VOXEL_CORE_ASSERT(!s_instance, "Application already exists!");

		s_instance = this;

		VOXEL_CORE_WARN("Application Name: {0}", spec.ApplicationName);
		VOXEL_CORE_WARN("Version: {0}", spec.Version);
		VOXEL_CORE_WARN("Working Directory: {0}", spec.WorkingDirectory);
		VOXEL_CORE_WARN("Command Line Args: {0}", spec.CommandLineArgs.toString());

		setupInputCallbacks();

		std::stringstream name;
		string graphicsSpec = renderer::graphicsSpecString(renderer::g_graphicsSpec);
		name << spec.ApplicationName << " " << spec.Version << " (" << graphicsSpec << ")";
		m_window = Window::Create({ name.str(), 1920, 1080 });
		m_window->setEventCallback(BIND_CALLBACK(onEvent));
		m_window->setMaximized(spec.Maximized);
	}

	inline constexpr Application& Application::getInstance()
	{
		return *s_instance;
	}
	inline constexpr float& Application::getDeltaTime()
	{
		return m_frameTimer;
	}
	inline constexpr uint32& Application::getFPS()
	{
		return m_lastFPS;
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
			VOXEL_CORE_WARN("Application initialization.");
			g_imguiLayer = new renderer::ImGuiLayer();
			pushOverlay(g_imguiLayer);
			renderer::Renderer::init(*m_window.get());
		}
		catch (const std::exception& e)
		{
			VOXEL_CORE_CRITICAL(e.what());
			VOXEL_DEBUGBREAK();
		}
	}
	void Application::run()
	{
		VOXEL_CORE_WARN("Running {0}...", m_specification.ApplicationName);

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

	void Application::setupInputCallbacks()
	{
		m_dispatcher.registerEvent<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		m_dispatcher.registerEvent<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
	}
	void Application::nextFrame()
	{
		auto tStart = std::chrono::high_resolution_clock::now();

		if (!m_minimized)
		{
			m_layerStack.onUpdate(m_frameTimer);

			m_accumulator += m_frameTimer;
			while (m_accumulator >= fixedDeltaTime)
			{
				m_layerStack.onFixedUpdate(fixedDeltaTime);
				m_accumulator -= fixedDeltaTime;
			}

			g_imguiLayer->preRender();
			m_layerStack.onImGuiRender();
			g_imguiLayer->postRender();

			m_frameCounter++;

			auto tEnd = std::chrono::high_resolution_clock::now();
			auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
			m_frameTimer = (float)tDiff / 1000.0f;

			calculateFramerate(tEnd);
		}
	}
	void Application::calculateFramerate(const std::chrono::steady_clock::time_point& tEnd)
	{
		float fpsTimer = (float)(std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count());
		if (fpsTimer > 1000.0f)
		{
			m_lastFPS = static_cast<uint32>((float)m_frameCounter * (1000.0f / fpsTimer));
			m_frameCounter = 0;
			lastTimestamp = tEnd;
		}
		tPrevEnd = tEnd;
	}
	void Application::onEvent(input::Event& e)
	{
		m_dispatcher.dispatchEvent(e, std::launch::async);
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
#pragma once
#include <InputCore/events/ApplicationEvent.h>
#include <InputCore/events/EventDispatcher.h>
#include "LayerStack.h"
#include "imgui/ImGuiLayer.h"
#include <Core/Logging/Assert.h>
#include <Core/HAL/Platform/Window.h>

struct ApplicationCommandLineArgs
{
	int Count = 0;
	char** Args = nullptr;

	constexpr const char* operator[](int index) const
	{
		RUNTIME_ASSERT(index < Count, "Invalid index when accessing to command line arguments.");
		return Args[index];
	}
	constexpr string str() const
	{
		string args;
		for (int i = 0; i < Count; ++i)
		{
			args += Args[i];
			args += ";";
		}
		return args;
	}
};
struct ApplicationSpecification
{
	string ApplicationName;
	string WorkingDirectory;
	string Version;
	bool Maximized = false;
	ApplicationCommandLineArgs CommandLineArgs;
};

namespace VoxelEngine
{
	class Application
	{
	private:
		ApplicationSpecification m_specification;
		TUniquePtr<Window> m_window;
		renderer::LayerStack m_layerStack;
		bool m_running = false;
		bool m_minimized = false;
		input::EventDispatcher m_eventDispatcher;

		uint32 m_frameCounter = 0;
		uint32 m_lastFPS = 0;
		double m_accumulator = 0.0;
		double m_frameTimer = 1.0;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;

		static constexpr double fixedDeltaTime = 1.0 / 60.0;

		static Application* s_instance;
		static renderer::ImGuiLayer* s_imguiLayer;

		void setupInputCallbacks();
		void nextFrame();
		void calculateFramerate(const std::chrono::steady_clock::time_point& tEnd);

		void onEvent(input::Event& e);
		bool onWindowClose(const input::WindowCloseEvent& e);
		bool onWindowResize(const input::WindowResizeEvent& e);
	protected:
		Application(const ApplicationSpecification& spec);
		void pushLayer(renderer::Layer* layer);
		void pushOverlay(renderer::Layer* layer);
	public:
		Application() noexcept = delete;
		Application(const Application&) noexcept = delete;
		Application(Application&&) noexcept = delete;
		Application& operator=(Application const& rhs) noexcept = delete;
		Application& operator=(Application&& rhs) noexcept = delete;

		FORCE_INLINE static Application* getInstance() { return s_instance; }
		FORCE_INLINE static renderer::ImGuiLayer* getImGuiLayer() { return s_imguiLayer; }
		FORCE_INLINE const float& getDeltaTime() const { return m_frameTimer; }
		FORCE_INLINE const uint32& getFPS() const { return m_lastFPS; }
		FORCE_INLINE const TUniquePtr<Window>& getWindow() const { return m_window; }

		void init();
		void run();
		void shutdown();

		virtual ~Application();
	};
}
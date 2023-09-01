#pragma once
#include "input/events/ApplicationEvent.h"
#include "input/events/EventDispatcher.h"
#include "Assert.h"
#include "LayerStack.h"
#include "renderer/Renderer.h"
#include "imgui/ImGuiLayer.h"

struct ApplicationCommandLineArgs
{
	int Count = 0;
	char** Args = nullptr;

	constexpr const char* operator[](int index) const
	{
		VOXEL_CORE_ASSERT(index < Count, "Invalid index when accessing to command line arguments.");
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
	class VOXEL_API Application
	{
	private:
		ApplicationSpecification m_specification;
		UniqueRef<Window> m_window;
		renderer::LayerStack m_layerStack;
		input::EventDispatcher m_dispatcher;
		bool m_running = false;
		bool m_minimized = false;

		uint32 m_frameCounter = 0;
		uint32 m_lastFPS = 0;
		float m_accumulator = 0.0f;
		float m_frameTimer = 1.0f;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;

		static constexpr double fixedDeltaTime = 1 / 60.0f;

		static Application* s_instance;

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

		INLINE static Application& getInstance();
		INLINE const float& getDeltaTime() const;
		INLINE const uint32& getFPS() const;

		INLINE const UniqueRef<Window>& getWindow() const { return m_window; }

		void init();
		void run();
		void shutdown();

		virtual ~Application() = default;
	};
}
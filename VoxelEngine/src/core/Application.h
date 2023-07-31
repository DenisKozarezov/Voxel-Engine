#pragma once
#include "input/events/ApplicationEvent.h"
#include "input/events/EventDispatcher.h"
#include "Window.h"
#include "Assert.h"
#include "LayerStack.h"

struct ApplicationCommandLineArgs
{
	int Count = 0;
	char** Args = nullptr;

	const char* operator[](int index) const
	{
		VOXEL_CORE_ASSERT(index < Count, "Invalid index when accessing to command line arguments.")
		return Args[index];
	}
	const std::string toString() const
	{
		std::stringstream args;
		for (int i = 0; i < Count; ++i)
		{
			args << Args[i];
			args << ";";
		}
		return args.str();
	}
};
struct ApplicationSpecification
{
	std::string ApplicationName;
	std::string WorkingDirectory;
	std::string Version;
	std::string GraphicsAPI;
	bool Maximized = false;
	ApplicationCommandLineArgs CommandLineArgs;
};

namespace VoxelEngine
{
	class VOXEL_API Application
	{
	private:
		ApplicationSpecification _specification;
		UniqueRef<Window> _window;
		renderer::LayerStack _layerStack;
		input::EventDispatcher _dispatcher;
		bool _running = false;
		bool _minimized = false;

		uint32_t _frameCounter = 0;
		uint32_t _lastFPS = 0;
		float _accumulator = 0.0f;
		float _frameTimer = 1.0f;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;

		static constexpr double fixedDeltaTime = 1 / 60.0f;

		static Application* _instance;

		void setupInputCallbacks();
		void nextFrame();

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
		Application& operator= (Application const& rhs) noexcept = delete;
		Application& operator= (Application&& rhs) noexcept = delete;

		static Application& getInstance();
		const float& getDeltaTime() const;	
		const uint32& getFPS() const;

		void init();
		void run();
		void shutdown();

		virtual ~Application() = default;
	};
}
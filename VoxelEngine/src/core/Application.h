#pragma once
#include "platform/Vulkan/VulkanRenderer.h"
#include "input/events/ApplicationEvent.h"
#include "Log.h"
#include "Assert.h"
#include "Window.h"
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
	ApplicationCommandLineArgs CommandLineArgs;
};

namespace VoxelEngine
{
#define BIND_CALLBACK(method) std::bind(&Application::method, this, std::placeholders::_1)

	class VOXEL_API Application
	{
	private:
		ApplicationSpecification _specification;
		UniqueRef<Window> _window;
		renderer::LayerStack _layerStack;
		renderer::VulkanRenderer _renderer;
		bool _running = false;
		bool _minimized;
		float _lastFrameTime = 0.0f;
		static Application* _instance;

		void onEvent(input::Event& e);
		bool onWindowClose(const input::WindowCloseEvent& e);
		bool onWindowResize(const input::WindowResizeEvent& e);
	protected:
		Application(const ApplicationSpecification& spec);
		void pushLayer(const SharedRef<renderer::Layer>& layer);
		void pushOverlay(const SharedRef<renderer::Layer>& layer);
	public:
		Application() = delete;
		Application(const Application&) = delete;
		Application(Application&&) = delete;

		inline static const SharedRef<Application> getInstance();

		const void init();
		const void run();
		const void shutdown();

		virtual ~Application() = default;
	};
}
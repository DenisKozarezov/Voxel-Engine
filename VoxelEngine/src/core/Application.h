#pragma once
#include "renderer/Renderer.h"
#include "input/events/ApplicationEvent.h"

namespace VoxelEngine
{
	class VOXEL_API Application
	{
	private:
		GLFWwindow* _window = nullptr;
		renderer::Renderer _renderer;
		bool _running = false;
		static Application* _instance;

		GLFWwindow* const createWindow(const int& width, const int& height, const string& title) const noexcept;
	protected:
		Application();
	public:
		Application(const Application&) = delete;
		Application(const std::shared_ptr<Application>&) = delete;
		Application(const std::unique_ptr<Application>&) = delete;
		const Application& operator=(const Application&) = delete;

		inline static std::unique_ptr<Application>& getInstance();

		const void init();
		const void run();
		const void shutdown();

		void onEvent(const input::Event& e);
		void onWindowClosed(const input::WindowCloseEvent& e);
		void onWindowResized(const input::WindowResizeEvent& e);

		virtual ~Application() = default;
	};
}
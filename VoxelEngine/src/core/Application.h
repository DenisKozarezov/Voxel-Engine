#pragma once
#include "renderer/Renderer.h"
#include "input/events/ApplicationEvent.h"

int main(int argc, char** argv);

namespace VoxelEngine
{
	class VOXEL_API Application
	{
	private:
		GLFWwindow* _window = nullptr;
		renderer::Renderer _renderer;
		bool _running = false;

		friend int ::main(int argc, char** argv);
		GLFWwindow* const createWindow(const int& width, const int& height, const string& title) const noexcept;
	public:
		Application() = default;
		Application(const Application&) = delete;
		Application(const std::shared_ptr<Application>&) = delete;
		Application(const std::unique_ptr<Application>&) = delete;
		Application(const unsigned int& width, const unsigned int& height, const string& title);
		const Application& operator=(const Application&) = delete;

		const void init();
		const void run();
		const void shutdown();

		void onEvent(const input::Event& e);
		void onWindowClosed(const input::WindowCloseEvent& e);
		void onWindowResized(const input::WindowResizeEvent& e);

		virtual ~Application() = default;
	};
}
#pragma once
#include "renderer/Renderer.h"
#include <string>

namespace core
{
	class Application
	{
	private:
		GLFWwindow* _window = nullptr;
		renderer::Renderer _renderer;
		bool _running = false;

		GLFWwindow* const createWindow(const int& width, const int& height, const std::string& title) const noexcept;
	public:
		Application() = default;

		const void init();
		const void run();
		const void shutdown();

		~Application();
	};
}
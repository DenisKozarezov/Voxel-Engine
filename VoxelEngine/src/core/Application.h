#pragma once
#include "input/events/ApplicationEvent.h"
#include "input/events/KeyboardEvent.h"
#include "input/events/MouseEvent.h"
#include "input/events/EventDispatcher.h"
#include "Window.h"
#include "Assert.h"
#include "LayerStack.h"
#include "components/camera/FirstPersonCamera.h"

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
#define BIND_CALLBACK(method) std::bind(&Application::method, this, std::placeholders::_1)

	class VOXEL_API Application
	{
	private:
		ApplicationSpecification _specification;
		UniqueRef<Window> _window;
		renderer::LayerStack _layerStack;
		input::EventDispatcher _dispatcher;
		components::camera::FirstPersonCamera _camera{glm::vec3(0.0f, 0.0f, 5.0f)};
		bool _running = false;
		bool _minimized = false;
		input::MouseDraggingState _mouseState;
		float _lastMouseX = 0.0f, _lastMouseY = 0.0f;
		float _deltaTime;
		static Application* _instance;

		void moveCamera(const components::camera::CameraMovement& direction);
		void mouseMove(const float& x, const float& y);
		void setMouseDragging(const bool& isDragging);
		void setupInputCallbacks();

		void onEvent(input::Event& e);
		bool onWindowClose(const input::WindowCloseEvent& e);
		bool onWindowResize(const input::WindowResizeEvent& e);
		bool onKeyboardPressed(const input::KeyPressedEvent& e);
		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);
		bool onMouseMoved(const input::MouseMovedEvent& e);
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

		void init();
		void run();
		void shutdown();

		virtual ~Application() = default;
	};
}
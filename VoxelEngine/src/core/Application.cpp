#include "Application.h"
#include "renderer/Renderer.h"
#include "imgui/ImGuiLayer.h"
#include "Timer.h"

namespace VoxelEngine
{
	Application* Application::_instance = 0;

	Application::Application(const ApplicationSpecification& spec) : _specification(spec)
	{
		VOXEL_CORE_ASSERT(!_instance, "Application already exists!")

		_instance = this;

		VOXEL_CORE_WARN("Application Name: {0}", spec.ApplicationName)
		VOXEL_CORE_WARN("Version: {0}", spec.Version)
		VOXEL_CORE_WARN("Working Directory: {0}", spec.WorkingDirectory)
		VOXEL_CORE_WARN("Command Line Args: {0}", spec.CommandLineArgs.toString())

		setupInputCallbacks();

		std::stringstream name;
		name << spec.ApplicationName << " " << spec.Version << " (" << spec.GraphicsAPI << ")";
		_window = Window::Create({ name.str(), 1920, 1080 });
		_window->setEventCallback(BIND_CALLBACK(onEvent));
		_window->setMaximized(spec.Maximized);
	}

	Application& Application::getInstance()
	{
		return *_instance;
	}
	const float& Application::getDeltaTime() const
	{
		return _deltaTime;
	}

	void Application::pushLayer(renderer::Layer* layer)
	{
		_layerStack.pushLayer(layer);
		layer->onAttach();
	}
	void Application::pushOverlay(renderer::Layer* layer)
	{
		_layerStack.pushOverlay(layer);
		layer->onAttach();
	}

	void Application::init()
	{
		try
		{
			pushOverlay(new renderer::ImGuiLayer());
			renderer::Renderer::init(*_window.get(), &_camera);
		}
		catch (const std::exception& e)
		{
			VOXEL_CORE_CRITICAL(e.what())
			VOXEL_DEBUGBREAK()
		}
	}
	void Application::run()
	{
		VOXEL_CORE_WARN("Running {0}...", _specification.ApplicationName)

		_running = true;
		Timer timer;

		while (_running)
		{
			if (!_minimized)
			{
				_layerStack.onUpdate(_deltaTime);

				timer.reset();

				renderer::Renderer::beginFrame();
				_layerStack.onImGuiRender();
				renderer::Renderer::endFrame();

				_deltaTime = timer.elapsedInMilliseconds<float>();
			}
			_window->onUpdate();
		}
		renderer::Renderer::deviceWaitIdle();
	}
	void Application::shutdown()
	{
		_layerStack.detach();
		renderer::Renderer::cleanup();
	}

	void Application::moveCamera(const components::camera::CameraMovement& direction)
	{
		_camera.processKeyboard(direction, _deltaTime);
	}
	void Application::mouseMove(const float& x, const float& y)
	{
		_camera.processMouse(x, y);
	}
	void Application::setMouseDragging(const bool& isDragging)
	{
		_mouseState = isDragging ? input::MouseDraggingState::DragBegin : input::MouseDraggingState::None;
	}
	void Application::setupInputCallbacks()
	{
		_dispatcher.registerEvent<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		_dispatcher.registerEvent<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
		_dispatcher.registerEvent<input::KeyPressedEvent>(BIND_CALLBACK(onKeyboardPressed));
		_dispatcher.registerEvent<input::MouseButtonPressedEvent>(BIND_CALLBACK(onMousePressed));
		_dispatcher.registerEvent<input::MouseButtonReleasedEvent>(BIND_CALLBACK(onMouseReleased));
		_dispatcher.registerEvent<input::MouseMovedEvent>(BIND_CALLBACK(onMouseMoved));
	}
	void Application::onEvent(input::Event& e)
	{
		_dispatcher.dispatchEvent(e, std::launch::async);
	}
	bool Application::onWindowClose(const input::WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}
	bool Application::onWindowResize(const input::WindowResizeEvent& e)
	{
		if (e.getWidth() == 0 || e.getHeight() == 0)
		{
			_minimized = true;
			return false;
		}
		_minimized = false;
		return false;
	}
	bool Application::onKeyboardPressed(const input::KeyPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::W:
			moveCamera(components::camera::CameraMovement::Forward);
			break;
		case input::S:
			moveCamera(components::camera::CameraMovement::Backward);
			break;
		case input::A:
			moveCamera(components::camera::CameraMovement::Left);
			break;
		case input::D:
			moveCamera(components::camera::CameraMovement::Right);
			break;
		}
		return true;
	}
	bool Application::onMousePressed(const input::MouseButtonPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(true);
			break;
		}
		return true;
	}
	bool Application::onMouseReleased(const input::MouseButtonReleasedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(false);
			break;
		}
		return true;
	}
	bool Application::onMouseMoved(const input::MouseMovedEvent& e)
	{
		const float x = e.getX();
		const float y = e.getY();

		if (_mouseState == input::MouseDraggingState::DragBegin)
		{
			_lastMouseX = x;
			_lastMouseY = y;
			_mouseState = input::MouseDraggingState::Dragging;
		}

		if (_mouseState == input::MouseDraggingState::Dragging)
		{
			mouseMove(x - _lastMouseX, _lastMouseY - y);
			_lastMouseX = x;
			_lastMouseY = y;
		}
		return true;
	}
}
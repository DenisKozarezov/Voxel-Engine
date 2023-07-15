#include "Application.h"
#include "input/events/EventBus.h"
#include "Timestep.h"
#include "renderer/Renderer.h"
#include "imgui/ImGuiLayer.h"

namespace VoxelEngine
{
	Application* Application::_instance = 0;
	float Application::_deltaTime = 0.0f;

	Application::Application(const ApplicationSpecification& spec) : _specification(spec)
	{
		VOXEL_CORE_ASSERT(!_instance, "Application already exists!")

		_instance = this;

		VOXEL_CORE_WARN("Application Name: " + spec.ApplicationName)
		VOXEL_CORE_WARN("Version: " + spec.Version)
		VOXEL_CORE_WARN("Working Directory: " + spec.WorkingDirectory)
		VOXEL_CORE_WARN("Command Line Args: " + spec.CommandLineArgs.toString())

		std::stringstream name;
		name << spec.ApplicationName << " " << spec.Version << " (" << spec.GraphicsAPI << ")";
		_window = Window::Create({ name.str(), 1920, 1080 });
		_window->setEventCallback(BIND_CALLBACK(onEvent));
		_window->setMaximized(spec.Maximized);
	}

	const SharedRef<Application> Application::getInstance()
	{
		return SharedRef<Application>(_instance);
	}
	const float& Application::getDeltaTime()
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

		while (_running)
		{
			const float time = renderer::Renderer::getTime();
			const float deltaTime = time - _lastFrameTime;
			_deltaTime = deltaTime;
			_lastFrameTime = time;

			if (!_minimized)
			{
				_layerStack.onUpdate(deltaTime);
				renderer::Renderer::beginFrame();
				_layerStack.onImGuiRender();
				renderer::Renderer::endFrame();
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

	void Application::onEvent(input::Event& e)
	{
		input::EventBus dispatcher(e);
		dispatcher.Fire<input::WindowCloseEvent>(BIND_CALLBACK(onWindowClose));
		dispatcher.Fire<input::WindowResizeEvent>(BIND_CALLBACK(onWindowResize));
		dispatcher.Fire<input::KeyPressedEvent>(BIND_CALLBACK(onKeyboardPressed));
		dispatcher.Fire<input::MouseButtonPressedEvent>(BIND_CALLBACK(onMousePressed));
		dispatcher.Fire<input::MouseButtonReleasedEvent>(BIND_CALLBACK(onMouseReleased));
		dispatcher.Fire<input::MouseMovedEvent>(BIND_CALLBACK(onMouseMoved));
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
			mouseMove(x - _lastMouseX, y - _lastMouseY);
			_lastMouseX = x;
			_lastMouseY = y;
		}
		return true;
	}
}
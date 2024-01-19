#pragma once
#include <Core/pch.h>
#include <InputCore/events/Event.h>

namespace VoxelEngine
{
	struct WindowProperties final
	{
		string Title;
		uint16 Width;
		uint16 Height;

		constexpr WindowProperties(const string& title, const uint16& width, const uint16& height)
			: Title(title), Width(width), Height(height) { }
		constexpr WindowProperties(const WindowProperties& props)
			: Title(props.Title), Width(props.Width), Height(props.Height) { }

		~WindowProperties() = default;
	};

	class Window
	{
	public:
		using EventCallback = std::function<void(input::Event&)>;

		Window() = default;
		Window(Window&&) noexcept = delete;
		Window(Window const&) noexcept = delete;
		Window& operator= (Window const& rhs) noexcept = delete;
		Window& operator= (Window&& rhs) noexcept = delete;

		virtual INLINE const uint16& getWidth() const = 0;
		virtual INLINE const uint16& getHeight() const = 0;
		virtual INLINE const void* getNativeWindow() const & noexcept = 0;
		virtual void setEventCallback(const EventCallback& callback) noexcept = 0;
		virtual void setMaximized(const bool& isMaximized) = 0;
		virtual void init() = 0;
		virtual void shutdown() = 0;
		virtual void onUpdate() = 0;
		virtual void waitEvents() const = 0;

		// Defined in client platforms
		static TUniquePtr<Window> Create(const WindowProperties& props);

		virtual ~Window() = default;
	};
}
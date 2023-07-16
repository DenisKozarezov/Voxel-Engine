#pragma once
#include <pch.h>
#include "Event.h"

namespace VoxelEngine::input
{
	template<typename T, typename F>
	concept is_event = requires(T t, F && f)
	{
		requires std::is_base_of_v<Event, T> && !std::is_abstract_v<T>;
		{ f(t) } -> std::same_as<bool>;
	};

	class EventBus final
	{
	private:
		Event& _event;
	public:
		EventBus() noexcept = default;
		EventBus(const EventBus&) noexcept = delete;
		EventBus(EventBus&&) noexcept = delete;
		EventBus& operator= (EventBus const& rhs) noexcept = delete;
		EventBus& operator= (EventBus&& rhs) noexcept = delete;
		EventBus(Event& event) : _event(event) { }

		template<typename T, typename F>
		requires is_event<T, F>
		bool Fire(const F&& func)
		{
			if (_event.eventType() == T::getStaticType())
			{
				_event.Handled |= func(static_cast<T&>(_event));
				return true;
			}
			return false;
		}

		~EventBus() noexcept = default;
	};	
}
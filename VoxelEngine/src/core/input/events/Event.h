#pragma once
#include <core/Base.h>
#include <core/PrimitiveTypes.h>

namespace VoxelEngine::input
{
	enum class EventType : byte
	{
		None = 0,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
		AppTick,
		AppUpdate,
		AppRender,
		KeyPressed,
		KeyReleased,
		KeyTyped,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled
	};

	enum EventCategory : byte
	{
		None						= 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

#define EVENT_CLASS_TYPE(type)	static const EventType& getStaticType() { return EventType::type; }				\
								virtual const EventType& eventType() const override { return getStaticType(); }	\
								virtual const string getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual const int& categoryFlags() const override { return category; }

	class VOXEL_API Event
	{
	private:
		bool _handled = false;
	public:
		Event() = default;
		Event(const Event&) = delete;

		virtual const EventType& eventType() const = 0;
		virtual const string getName() const = 0;
		virtual const int& categoryFlags() const = 0;
		virtual const string toString() const { return getName(); }
		inline const bool isInCategory(const EventCategory& category) const
		{
			return categoryFlags() & category;
		}
		inline const bool isHandled() const { return _handled; }

		virtual ~Event() = default;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}
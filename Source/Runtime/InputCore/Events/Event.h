#pragma once
#include <Core/Base.h>
#include <Core/CoreTypes.h>
#include <sstream>

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
		MouseScrolled,
		MeshLoaded
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

#define EVENT_CLASS_TYPE(type)	INLINE static const EventType getStaticType() { return EventType::type; }				\
								INLINE virtual const EventType eventType() const override { return getStaticType(); }	\
								INLINE virtual const string getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual const int categoryFlags() const override { return category; }

	class VOXEL_API Event
	{
	public:
		bool Handled = false;
		
		Event() = default;
		Event(const Event&) = delete;

		INLINE virtual const EventType eventType() const = 0;
		INLINE virtual const string getName() const = 0;
		virtual const int categoryFlags() const = 0;
		virtual const string str() const { return getName(); }
		INLINE const bool hasCategoryFlag(const EventCategory& category) const
		{
			return categoryFlags() & category;
		}

		virtual ~Event() = default;
	};

	INLINE std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.str();
	}
}
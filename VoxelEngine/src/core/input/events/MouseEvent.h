#pragma once
#include "Event.h"
#include "MouseCode.h"

namespace VoxelEngine::input
{
    enum class MouseDraggingState { None, DragBegin, Dragging, DragEnd };

    class MouseButtonEvent : public Event
    {
    protected:
        MouseCode m_mouseCode;
    public:
        MouseButtonEvent() noexcept = default;
        MouseButtonEvent(const MouseCode& key) : m_mouseCode(key) { }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

        INLINE const MouseCode& getKeyCode() const { return m_mouseCode; }
        const string str() const override
        {
            std::stringstream ss;
            ss << this->getName() << ": " << m_mouseCode;
            return ss.str();
        }

        ~MouseButtonEvent() noexcept = default;
    };

    class MouseMovedEvent final : public MouseButtonEvent
    {
    private:
        float m_posX, m_posY;
    public:
        MouseMovedEvent() = delete;
        MouseMovedEvent(const MouseMovedEvent& e) : m_posX(e.m_posX), m_posY(e.m_posY) { }
        MouseMovedEvent(const float& x, const float& y) : m_posX(x), m_posY(y) { }

        INLINE const float& getX() const { return m_posX; }
        INLINE const float& getY() const { return m_posY; }

        const string str() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_posX << ", " << m_posY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)

        ~MouseMovedEvent() noexcept = default;
    };

    class MouseScrolledEvent final : public MouseButtonEvent
    {
    private:
        float m_offsetX, m_offsetY;
    public:
        MouseScrolledEvent() noexcept = delete;
        MouseScrolledEvent(const MouseScrolledEvent& e) : m_offsetX(e.m_offsetX), m_offsetY(e.m_offsetY) { }
        MouseScrolledEvent(const float& offsetX, const float& offsetY) : m_offsetX(offsetX), m_offsetY(offsetY) { }

        INLINE const float& getOffsetX() const { return m_offsetX; }
        INLINE const float& getOffsetY() const { return m_offsetY; }

        EVENT_CLASS_TYPE(MouseScrolled)

        ~MouseScrolledEvent() noexcept = default;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent() noexcept = delete;
        MouseButtonPressedEvent(const MouseCode& key) : MouseButtonEvent(key) { }

        EVENT_CLASS_TYPE(MouseButtonPressed)

        ~MouseButtonPressedEvent() noexcept = default;
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent() noexcept = delete;
        MouseButtonReleasedEvent(const MouseCode& key) : MouseButtonEvent(key) { }

        EVENT_CLASS_TYPE(MouseButtonReleased)

        ~MouseButtonReleasedEvent() noexcept = default;
    };
}
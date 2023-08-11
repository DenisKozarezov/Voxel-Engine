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

        inline const MouseCode& getKeyCode() const & { return m_mouseCode; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << this->getName() << ": " << m_mouseCode;
            return ss.str();
        }

        ~MouseButtonEvent() = default;
    };

    class MouseMovedEvent final : public MouseButtonEvent
    {
    private:
        float m_posX, m_posY;
    public:
        MouseMovedEvent() = delete;
        MouseMovedEvent(const MouseMovedEvent& e) : m_posX(e.m_posX), m_posY(e.m_posY) { }
        MouseMovedEvent(const float& x, const float& y) : m_posX(x), m_posY(y) { }

        inline const float& getX() const { return m_posX; }
        inline const float& getY() const { return m_posY; }

        const string toString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_posX << ", " << m_posY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)

        ~MouseMovedEvent() = default;
    };

    class MouseScrolledEvent final : public MouseButtonEvent
    {
    private:
        float m_offsetX, m_offsetY;
    public:
        MouseScrolledEvent() = delete;
        MouseScrolledEvent(const MouseScrolledEvent& e) : m_offsetX(e.m_offsetX), m_offsetY(e.m_offsetY) { }
        MouseScrolledEvent(const float& offsetX, const float& offsetY) : m_offsetX(offsetX), m_offsetY(offsetY) { }

        inline const float& getOffsetX() const { return m_offsetX; }
        inline const float& getOffsetY() const { return m_offsetY; }

        EVENT_CLASS_TYPE(MouseScrolled)

        ~MouseScrolledEvent() = default;
    };

    class MouseButtonPressedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonPressedEvent() = delete;
        MouseButtonPressedEvent(const MouseCode& key) : MouseButtonEvent(key) { }

        EVENT_CLASS_TYPE(MouseButtonPressed)

        ~MouseButtonPressedEvent() = default;
    };

    class MouseButtonReleasedEvent final : public MouseButtonEvent
    {
    public:
        MouseButtonReleasedEvent() = delete;
        MouseButtonReleasedEvent(const MouseCode& key) : MouseButtonEvent(key) { }

        EVENT_CLASS_TYPE(MouseButtonReleased)

        ~MouseButtonReleasedEvent() = default;
    };
}
#pragma once
#include "Event.h"
#include "MouseCode.h"

namespace VoxelEngine::input
{
    enum class MouseDraggingState { None, DragBegin, Dragging, DragEnd };

    class MouseButtonEvent : public Event
    {
    protected:
        MouseCode _mouseCode;
    public:
        MouseButtonEvent() = default;
        MouseButtonEvent(const MouseCode& key) : _mouseCode(key) { }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

        inline const MouseCode& getKeyCode() const & { return _mouseCode; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << this->getName() << ": " << _mouseCode;
            return ss.str();
        }

        ~MouseButtonEvent() = default;
    };

    class MouseMovedEvent final : public MouseButtonEvent
    {
    private:
        float _posX, _posY;
    public:
        MouseMovedEvent() = delete;
        MouseMovedEvent(const MouseMovedEvent& e) : _posX(e._posX), _posY(e._posY) { }
        MouseMovedEvent(const float& x, const float& y) : _posX(x), _posY(y) { }

        inline const float& getX() const { return _posX; }
        inline const float& getY() const { return _posY; }

        const string toString() const override
        {
            std::stringstream ss;
            ss << "MouseMovedEvent: " << _posX << ", " << _posY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)

        ~MouseMovedEvent() = default;
    };

    class MouseScrolledEvent final : public MouseButtonEvent
    {
    private:
        float _offsetX, _offsetY;
    public:
        MouseScrolledEvent() = delete;
        MouseScrolledEvent(const MouseScrolledEvent& e) : _offsetX(e._offsetX), _offsetY(e._offsetY) { }
        MouseScrolledEvent(const float& offsetX, const float& offsetY) : _offsetX(offsetX), _offsetY(offsetY) { }

        inline const float& getOffsetX() const { return _offsetX; }
        inline const float& getOffsetY() const { return _offsetY; }

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
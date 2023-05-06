#pragma once
#include "Event.h"

namespace VoxelEngine::input
{
    class WindowResizeEvent final : public Event
    {
    private:
        uint16 _width, _height;
    public:
        WindowResizeEvent(uint16 width, uint16 height) : _width(width), _height(height) { }
        WindowResizeEvent(const WindowResizeEvent&) = delete;

        inline const uint16& getWidth() const { return _width; }
        inline const uint16& getHeight() const { return _height; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << _width << ", " << _height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~WindowResizeEvent() = default;
    };

    class WindowCloseEvent final : public Event
    {
    public:
        WindowCloseEvent() = default;
        WindowCloseEvent(const WindowCloseEvent&) = delete;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~WindowCloseEvent() = default;
    };

    class ApplicationUpdateEvent final : public Event
    {
        ApplicationUpdateEvent() = default;
        ApplicationUpdateEvent(const ApplicationUpdateEvent&) = delete;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~ApplicationUpdateEvent() = default;
    };
}
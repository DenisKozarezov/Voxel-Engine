#pragma once
#include "Event.h"

namespace VoxelEngine::input
{
    class WindowResizeEvent final : public Event
    {
    private:
        uint16 m_width, m_height;
    public:
        WindowResizeEvent(uint16 width, uint16 height) : m_width(width), m_height(height) { }
        WindowResizeEvent(const WindowResizeEvent&) = delete;

        inline const uint16& getWidth() const { return m_width; }
        inline const uint16& getHeight() const { return m_height; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_width << ", " << m_height;
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
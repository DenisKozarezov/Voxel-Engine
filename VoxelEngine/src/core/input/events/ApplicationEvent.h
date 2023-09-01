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
        WindowResizeEvent(const WindowResizeEvent&) noexcept = delete;

        INLINE const uint16& getWidth() const { return m_width; }
        INLINE const uint16& getHeight() const { return m_height; }
        const string str() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_width << ", " << m_height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~WindowResizeEvent() noexcept = default;
    };

    class WindowCloseEvent final : public Event
    {
    public:
        WindowCloseEvent() noexcept = default;
        WindowCloseEvent(const WindowCloseEvent&) noexcept = delete;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~WindowCloseEvent() noexcept = default;
    };

    class ApplicationUpdateEvent final : public Event
    {
        ApplicationUpdateEvent() noexcept = default;
        ApplicationUpdateEvent(const ApplicationUpdateEvent&) noexcept = delete;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

        ~ApplicationUpdateEvent() noexcept = default;
    };
}
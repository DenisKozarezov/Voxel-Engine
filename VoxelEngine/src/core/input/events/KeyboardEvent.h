#pragma once
#include "Event.h"
#include "KeyCode.h"

namespace VoxelEngine::input
{
    class KeyboardEvent : public Event
    {
    protected:
        KeyCode m_keyCode;
    public:
        KeyboardEvent() = delete;
        KeyboardEvent(const KeyboardEvent& e) : m_keyCode(e.m_keyCode) { }
        KeyboardEvent(const KeyCode& key) : m_keyCode(key) { }

        inline const KeyCode& getKeyCode() const & { return m_keyCode; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << this->getName() << ": " << m_keyCode;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

        virtual ~KeyboardEvent() = default;
    };

    class KeyPressedEvent final : public KeyboardEvent
    {
    private:
        bool m_isPressed;
    public:
        KeyPressedEvent() = delete;
        KeyPressedEvent(const KeyCode& key, bool isHold) : KeyboardEvent(key), m_isPressed(isHold) { }

        EVENT_CLASS_TYPE(KeyPressed)

        ~KeyPressedEvent() = default;
    };

    class KeyReleasedEvent final : public KeyboardEvent
    {
    public:
        KeyReleasedEvent() = delete;
        KeyReleasedEvent(const KeyCode& key) : KeyboardEvent(key) { }

        EVENT_CLASS_TYPE(KeyReleased)

        ~KeyReleasedEvent() = default;
    };

    class KeyTypedEvent final : public KeyboardEvent
    {
    public:
        KeyTypedEvent() = delete;
        KeyTypedEvent(const KeyCode& key) : KeyboardEvent(key) { }

        EVENT_CLASS_TYPE(KeyTyped)

        ~KeyTypedEvent() = default;
    };
}
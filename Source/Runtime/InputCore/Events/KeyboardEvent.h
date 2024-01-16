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
        KeyboardEvent() noexcept = delete;
        KeyboardEvent(const KeyboardEvent& e) : m_keyCode(e.m_keyCode) { }
        KeyboardEvent(const KeyCode& key) : m_keyCode(key) { }

        INLINE const KeyCode& getKeyCode() const { return m_keyCode; }
        const string str() const override
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
        KeyPressedEvent() noexcept = delete;
        KeyPressedEvent(const KeyCode& key, bool isHold) : KeyboardEvent(key), m_isPressed(isHold) { }

        EVENT_CLASS_TYPE(KeyPressed)

        ~KeyPressedEvent() override = default;
    };

    class KeyReleasedEvent final : public KeyboardEvent
    {
    public:
        KeyReleasedEvent() noexcept = delete;
        KeyReleasedEvent(const KeyCode& key) : KeyboardEvent(key) { }

        EVENT_CLASS_TYPE(KeyReleased)

        ~KeyReleasedEvent() override = default;
    };

    class KeyTypedEvent final : public KeyboardEvent
    {
    public:
        KeyTypedEvent() noexcept = delete;
        KeyTypedEvent(const KeyCode& key) : KeyboardEvent(key) { }

        EVENT_CLASS_TYPE(KeyTyped)

        ~KeyTypedEvent() override = default;
    };
}
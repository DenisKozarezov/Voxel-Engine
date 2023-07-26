#pragma once
#include "Event.h"
#include "KeyCode.h"

namespace VoxelEngine::input
{
    class KeyboardEvent : public Event
    {
    protected:
        KeyCode _keyCode;
    public:
        KeyboardEvent() = delete;
        KeyboardEvent(const KeyboardEvent& e) : _keyCode(e._keyCode) { }
        KeyboardEvent(const KeyCode& key) : _keyCode(key) { }

        inline const KeyCode& getKeyCode() const & { return _keyCode; }
        const string toString() const override
        {
            std::stringstream ss;
            ss << this->getName() << ": " << _keyCode;
            return ss.str();
        }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

        virtual ~KeyboardEvent() = default;
    };

    class KeyPressedEvent final : public KeyboardEvent
    {
    private:
        bool _isPressed;
    public:
        KeyPressedEvent() = delete;
        KeyPressedEvent(const KeyCode& key, bool isHold) : KeyboardEvent(key), _isPressed(isHold) { }

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
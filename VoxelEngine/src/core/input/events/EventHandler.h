#pragma once
#include <functional>
#include <typeinfo>

namespace VoxelEngine::input
{
    template<typename T>
    concept is_event = requires(T t)
    {
        requires std::is_base_of_v<Event, T> && !std::is_abstract_v<T>;
    };

    template<typename T, typename F>
    concept is_event_func = requires(T t, F && f)
    {
        requires is_event<T>;
        { f(t) } -> std::same_as<bool>;
    };

    class IEventHandler
    {
    public:
        virtual inline const size_t& getHashCode() const = 0;
        virtual void invoke(input::Event& arg) = 0;
    };

    template <typename TEvent>
    class EventHandler : public IEventHandler
    {
    public:
        using Callback = std::function<bool(TEvent&)>;

        explicit EventHandler(const Callback& cb) : _cbFunc(cb) 
        {
            _eventType = typeid(TEvent).hash_code();
        }
        
        ~EventHandler() noexcept = default;

        const bool operator==(const EventHandler& rhs)
        {
            return _eventType == rhs._eventType;
        }

        virtual inline const size_t& getHashCode() const override { return _eventType; }

        void invoke(input::Event& arg) override
        {            
            this->invoke(dynamic_cast<TEvent&>(arg));
        }
        void invoke(TEvent& arg)
        { 
            this->_cbFunc(std::forward<TEvent&>(arg));
        }
    private:
        size_t _eventType;
        Callback const _cbFunc;
    };
}
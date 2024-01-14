#pragma once
#include <functional>
#include <typeinfo>
#include <future>
#include <core/input/events/Event.h>

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
        requires is_event<T> && std::invocable<F&, const T&>;
        { f(t) } -> std::same_as<bool>;
    };

    class IEventHandler
    {
    public:
        IEventHandler() noexcept = default;
        virtual ~IEventHandler() noexcept = default;

        virtual INLINE const size_t& getHashCode() const = 0;
        virtual INLINE void invoke(input::Event& arg) = 0;
        virtual INLINE std::future<void> invoke_async(input::Event& arg, const std::launch& launch) = 0;
    };

    template <typename TEvent, typename TFunc>
    requires is_event_func<TEvent, TFunc>
    class EventHandler : public IEventHandler
    {
    public:
        EventHandler() noexcept = delete;
        explicit EventHandler(TFunc cb) : m_cbFunc(cb)
        {
            m_eventType = typeid(TEvent).hash_code();
        }       
        EventHandler(const EventHandler& src) : m_cbFunc(src.m_cbFunc), m_eventType(src.m_eventType)
        { }
        EventHandler(EventHandler&& src) : m_cbFunc(std::move(src.m_cbFunc)), m_eventType(src.m_eventType)
        { }
        ~EventHandler() noexcept = default;

        INLINE bool operator==(const EventHandler& rhs) noexcept
        {
            return m_eventType == rhs.m_eventType;
        }  
        EventHandler& operator=(const EventHandler& rhs) noexcept
        {
            if (this == &rhs)
                return this;

            m_cbFunc = rhs.m_cbFunc;
            m_eventType = rhs.m_eventType;
            return *this;
        }
        EventHandler& operator=(EventHandler&& rhs)
        {
            if (this == &rhs)
                return this;

            std::swap(m_cbFunc, rhs.m_cbFunc);
            m_eventType = rhs.m_eventType;
            return *this;
        }
        INLINE void operator()(TEvent& arg) const
        {
            invoke(arg);
        }

        INLINE const size_t& getHashCode() const override { return m_eventType; }

        INLINE void invoke(input::Event& arg) override
        {            
            this->invoke(static_cast<TEvent&>(arg));
        }
        INLINE std::future<void> invoke_async(input::Event& arg, const std::launch& launch) override
        {
            return this->invoke_async(static_cast<TEvent&>(arg), launch);
        }
        INLINE void invoke(TEvent& arg)
        { 
            this->m_cbFunc(std::forward<TEvent&>(arg));
        }
        std::future<void> invoke_async(TEvent& arg, const std::launch& launch)
        {
            return std::async(launch, [this, &arg] { this->invoke(arg); });
        }
    private:
        size_t m_eventType;
        TFunc const m_cbFunc;
    };
}
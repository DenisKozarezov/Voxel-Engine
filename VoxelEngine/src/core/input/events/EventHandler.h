#pragma once
#include <functional>
#include <typeinfo>
#include <future>

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
        requires is_event<T> && std::invocable<F>;
        { f(t) } -> std::same_as<bool>;
    };

    class IEventHandler
    {
    public:
        IEventHandler() noexcept = default;
        virtual ~IEventHandler() noexcept = default;

        virtual inline const size_t& getHashCode() const = 0;
        virtual void invoke(input::Event& arg) = 0;
        virtual std::future<void> invoke_async(input::Event& arg, const std::launch& launch) = 0;
    };

    template <typename TEvent>
    class EventHandler : public IEventHandler
    {
    public:
        using Callback = std::function<bool(TEvent&)>;

        EventHandler() noexcept = delete;
        explicit EventHandler(const Callback& cb) : m_cbFunc(cb) 
        {
            m_eventType = typeid(TEvent).hash_code();
        }       
        EventHandler(const EventHandler& src) : m_cbFunc(src.m_cbFunc), m_eventType(src.m_eventType)
        { }
        EventHandler(EventHandler&& src) : m_cbFunc(std::move(src.m_cbFunc)), m_eventType(src.m_eventType)
        { }
        ~EventHandler() noexcept = default;

        const bool operator==(const EventHandler& rhs) noexcept
        {
            return m_eventType == rhs.m_eventType;
        }  
        EventHandler& operator=(const EventHandler& src) noexcept
        {
            m_cbFunc = src.m_cbFunc;
            m_eventType = src.m_eventType;
            return *this;
        }
        EventHandler& operator=(EventHandler&& src)
        {
            std::swap(m_cbFunc, src.m_cbFunc);
            m_eventType = src.m_eventType;
            return *this;
        }
        inline void operator()(TEvent& arg) const
        {
            invoke(arg);
        }

        virtual inline const size_t& getHashCode() const override { return m_eventType; }

        void invoke(input::Event& arg) override
        {            
            this->invoke(dynamic_cast<TEvent&>(arg));
        }
        std::future<void> invoke_async(input::Event& arg, const std::launch& launch) override
        {
            return this->invoke_async(dynamic_cast<TEvent&>(arg), launch);
        }
        void invoke(TEvent& arg)
        { 
            this->m_cbFunc(std::forward<TEvent&>(arg));
        }
        std::future<void> invoke_async(TEvent& arg, const std::launch& launch)
        {
            return std::async(launch, [this, &arg] { this->invoke(arg); });
        }
    private:
        size_t m_eventType;
        Callback const m_cbFunc;
    };
}
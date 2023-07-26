#pragma once
#include <pch.h>
#include <mutex>
#include "EventHandler.h"

namespace VoxelEngine::input
{
    class EventDispatcher
    {
    public:
        EventDispatcher() noexcept = default;
        EventDispatcher(const EventDispatcher& src) noexcept
        {
            std::lock_guard<std::mutex> lock(src._handlersLocker);
            _eventList = src._eventList;
        }
        EventDispatcher(EventDispatcher&& src) noexcept
        {
            std::lock_guard<std::mutex> lock(src._handlersLocker);
            _eventList = std::move(src._eventList);
        }
        ~EventDispatcher()
        {
            for (const auto& el : _eventList)
            {
                for (const auto& cb : el.second)
                    delete cb;
            }
        }

        EventDispatcher& operator=(const EventDispatcher& src) noexcept
        {
            std::lock_guard<std::mutex> lock(_handlersLocker);
            std::lock_guard<std::mutex> lock2(src._handlersLocker);
            _eventList = src._eventList;
            return *this;
        }
        EventDispatcher& operator=(EventDispatcher&& src) noexcept
        {
            std::lock_guard<std::mutex> lock(_handlersLocker);
            std::lock_guard<std::mutex> lock2(src._handlersLocker);
            std::swap(_eventList, src._eventList);
            return *this;
        }

        template<typename TEvent>
        requires is_event<TEvent>
        constexpr void registerEvent(EventHandler<TEvent>::Callback callback)
        {
            if (callback)
            {
                registerEvent(new EventHandler<TEvent>(callback));
            }
        }
        void registerEvent(IEventHandler* event)
        {
            if (event)
            {
                std::lock_guard<std::mutex> lock(_handlersLocker);
                size_t hash = event->getHashCode();
                _eventList[hash].push_back(event);
            }
        }
        
        template<typename TEvent>
        requires is_event<TEvent>
        void unregisterEvent()
        {
            std::lock_guard<std::mutex> lock(_handlersLocker);

            size_t hash = typeid(TEvent).hash_code();
            if (_eventList.contains(hash))
            {
                for (const auto& cb : _eventList[hash])
                    delete cb;
                _eventList.erase(hash);
            }
        }
        void unregisterEvent(IEventHandler* event)
        {
            std::lock_guard<std::mutex> lock(_handlersLocker);

            if (event)
            {
                size_t hash = event->getHashCode();
                if (_eventList.contains(hash))
                {
                    for (const auto& cb : _eventList[hash])
                        delete cb;
                    _eventList.erase(hash);
                }
            }
        }

        template<typename TEvent>
        void dispatchEvent(TEvent& arg)
        {
            size_t hash = typeid(arg).hash_code();

            if (!_eventList.contains(hash))
                return;

            for (const auto& ie : _eventList[hash])
            {
                if (ie)
                {
                    ie->invoke(arg);
                }
            }
        }
        template<typename TEvent>
        void dispatchEvent(TEvent& arg, const std::launch& launch)
        {
            size_t hash = typeid(arg).hash_code();

            if (!_eventList.contains(hash))
                return;

            for (const auto& ie : _eventList[hash])
            {
                if (ie)
                {
                    std::future<void> future = ie->invoke_async(arg, launch);
                }
            }
        }
    private:
        typedef std::list<IEventHandler*> EventCallbacks;
        std::unordered_map<size_t, EventCallbacks> _eventList;
        mutable std::mutex _handlersLocker;
    };
}
#pragma once
#include <pch.h>
#include <mutex>
#include <core/Log.h>
#include "EventHandler.h"

namespace VoxelEngine::input
{
    class EventDispatcher
    {
    public:
        EventDispatcher() noexcept = default;
        EventDispatcher(const EventDispatcher& src) noexcept
        {
            std::lock_guard<std::mutex> lock(src.m_handlersLocker);
            m_eventList = src.m_eventList;
        }
        EventDispatcher(EventDispatcher&& src) noexcept
        {
            std::lock_guard<std::mutex> lock(src.m_handlersLocker);
            m_eventList = std::move(src.m_eventList);
        }
        ~EventDispatcher()
        {
            for (const auto& el : m_eventList)
            {
                for (const auto& cb : el.second)
                    delete cb;
            }
        }

        EventDispatcher& operator=(const EventDispatcher& src) noexcept
        {
            std::lock_guard<std::mutex> lock(m_handlersLocker);
            std::lock_guard<std::mutex> lock2(src.m_handlersLocker);
            m_eventList = src.m_eventList;
            return *this;
        }
        EventDispatcher& operator=(EventDispatcher&& src) noexcept
        {
            std::lock_guard<std::mutex> lock(m_handlersLocker);
            std::lock_guard<std::mutex> lock2(src.m_handlersLocker);
            std::swap(m_eventList, src.m_eventList);
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
                std::lock_guard<std::mutex> lock(m_handlersLocker);
                size_t hash = event->getHashCode();
                m_eventList[hash].push_back(event);
            }
        }
        
        template<typename TEvent>
        requires is_event<TEvent>
        void unregisterEvent()
        {
            std::lock_guard<std::mutex> lock(m_handlersLocker);

            size_t hash = typeid(TEvent).hash_code();
            if (m_eventList.contains(hash))
            {
                for (const auto& cb : m_eventList[hash])
                    delete cb;
                m_eventList.erase(hash);
            }
        }
        void unregisterEvent(IEventHandler* event)
        {
            std::lock_guard<std::mutex> lock(m_handlersLocker);

            if (event)
            {
                size_t hash = event->getHashCode();
                if (m_eventList.contains(hash))
                {
                    for (const auto& cb : m_eventList[hash])
                        delete cb;
                    m_eventList.erase(hash);
                }
            }
        }

        template<typename TEvent>
        void dispatchEvent(TEvent& arg)
        {
            size_t hash = typeid(arg).hash_code();

            if (!m_eventList.contains(hash))
                return;

            for (const auto& ie : m_eventList[hash])
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

            if (!m_eventList.contains(hash))
                return;

            for (const auto& ie : m_eventList[hash])
            {
                if (ie)
                {
                    std::future<void> future = ie->invoke_async(arg, launch);
                    
                    try
                    {
                        future.get();
                    }
                    catch (const std::exception& e)
                    {
                        VOXEL_CORE_CRITICAL(e.what())
                    }
                }
            }
        }
    private:
        typedef std::list<IEventHandler*> EventCallbacks;
        std::unordered_map<size_t, EventCallbacks> m_eventList;
        mutable std::mutex m_handlersLocker;
    };
}
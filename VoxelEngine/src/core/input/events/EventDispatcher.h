#pragma once
#include <pch.h>
#include <core/Log.h>
#include "EventHandler.h"

namespace VoxelEngine::input
{
    static class EventDispatcher
    {
    public:
        ~EventDispatcher()
        {
            for (const auto& el : m_eventList)
            {
                for (const auto& cb : el.second)
                    delete cb;
            }
        }

        template<typename TEvent>
        requires is_event<TEvent>
        constexpr static void registerEvent(EventHandler<TEvent>::Callback callback)
        {
            if (callback)
            {
                registerEvent(new EventHandler<TEvent>(callback));
            }
        }
        static void registerEvent(IEventHandler* event)
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
        static void unregisterEvent()
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
        static void unregisterEvent(IEventHandler* event)
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
        static void dispatchEvent(TEvent& arg)
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
        static void dispatchEvent(TEvent& arg, const std::launch& launch)
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
                        VOXEL_CORE_CRITICAL(e.what());
                    }
                }
            }
        }
    private:
        typedef std::list<IEventHandler*> EventCallbacks;
        typedef std::unordered_map<size_t, EventCallbacks> EventsList;
        static EventsList m_eventList;
        static std::mutex m_handlersLocker;
    };
}
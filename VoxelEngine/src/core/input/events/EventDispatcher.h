#pragma once
#include <pch.h>
#include "EventHandler.h"

namespace VoxelEngine::input
{
    class EventDispatcher
    {
    public:
        EventDispatcher() noexcept = default;
        ~EventDispatcher()
        {
            for (const auto& el : _eventList)
            {
                for (const auto& e : el.second)
                    delete e;
            }
        }

        template<typename TEvent>
        requires is_event<TEvent>
        void registerEvent(EventHandler<TEvent>::Callback callback)
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
                size_t hash = event->getHashCode();
                _eventList[hash].push_back(event);
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
    private:
        typedef std::vector<IEventHandler*> EventCallbacks;
        std::unordered_map<size_t, EventCallbacks> _eventList;
    };
}
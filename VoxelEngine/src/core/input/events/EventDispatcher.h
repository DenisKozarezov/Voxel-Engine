#pragma once
#include <core/PrimitiveTypes.h>
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
            for (auto el : _eventList)
            {
                for (auto e : el.second)
                    delete e;
            }
        }

        void registerEvent(IEventHandler* event)
        {
            if (event)
                _eventList[event->getName()].push_back(event);
        }

        template <typename ...Args>
        void dispatchEvent(const string& eventName, Args...args)
        {
            auto it_eventList = _eventList.find(eventName);
            if (it_eventList == _eventList.end())
                return;
            for (auto ie : it_eventList->second)
            {
                EventHandler<Args...>* event = dynamic_cast<EventHandler<Args...>*>(ie);
                if (event)
                {
                    event->invoke(args...);
                }
            }
        }

    private:
        std::map<string, std::vector<IEventHandler*>> _eventList;
    };
}
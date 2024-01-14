#include "EventDispatcher.h"

namespace VoxelEngine::input
{
    EventDispatcher::EventsList EventDispatcher::m_eventList = EventDispatcher::EventsList();
    std::mutex EventDispatcher::m_handlersLocker = std::mutex();
}
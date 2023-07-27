#include "WorkerThread.h"
#include <core/Log.h>

namespace VoxelEngine::threading
{
	WorkerThread::WorkerThread(const ThreadID& threadId, const Task& task) 
		: _threadId(threadId), _task(task)
	{
		VOXEL_CORE_WARN("Creating thread with ID = [{0}]...", threadId)

		_thread = std::make_unique<std::thread>([&task, &threadId]()-> void
		{
			task(threadId, 1);
		});
	}

	void WorkerThread::join() const
	{
		_thread->join();
	}

	WorkerThread::~WorkerThread()
	{
		VOXEL_CORE_WARN("Terminating thread with ID = [{0}]...", _threadId)
	}
}
#pragma once
#include <core/PrimitiveTypes.h>
#include <thread>
#include <condition_variable>
#include <functional>

namespace VoxelEngine::threading
{
	using ThreadID = uint32;
	using Task = std::function<void(ThreadID threadId, ThreadID threadsCountForTask)>;

	class WorkerThread final
	{
	private:
		ThreadID _threadId;
		Task _task;
		std::unique_ptr<std::thread> _thread;
		std::atomic_bool _busy = false;
	public:
		explicit WorkerThread(const ThreadID& threadId, const Task& task);

		WorkerThread() noexcept = delete;
		WorkerThread(WorkerThread const&) noexcept = delete;
		WorkerThread(WorkerThread&&) noexcept = delete;
		WorkerThread& operator=(WorkerThread const&) noexcept = delete;
		WorkerThread& operator=(WorkerThread&&) noexcept = delete;

		void join() const;

		~WorkerThread();
	};
}
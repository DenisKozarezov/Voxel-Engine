#pragma once
#include <core/PrimitiveTypes.h>
#include <thread>
#include <condition_variable>
#include <functional>

namespace VoxelEngine::threading
{
	using ThreadID = uint32;
	using Task = std::function<void(ThreadID threadId, ThreadID threadsCountForTask)>;

	class ThreadObject final
	{
	private:
		ThreadID _threadId;
		Task _task;
		std::unique_ptr<std::thread> _thread;
		std::atomic_bool _busy = false;
	public:
		explicit ThreadObject(const ThreadID& threadId, const Task& task);

		ThreadObject() noexcept = delete;
		ThreadObject(ThreadObject const&) noexcept = delete;
		ThreadObject(ThreadObject&&) noexcept = delete;
		ThreadObject& operator=(ThreadObject const&) noexcept = delete;
		ThreadObject& operator=(ThreadObject&&) noexcept = delete;

		void join() const;

		~ThreadObject();
	};
}
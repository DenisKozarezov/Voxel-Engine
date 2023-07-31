#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <core/PrimitiveTypes.h>
#include <core/Log.h>

namespace VoxelEngine::threading
{
	using Task = std::function<void()>;

	class ThreadWorker
	{
	private:
		bool destroying = false;
		int workerId;
		std::thread worker;
		std::queue<Task> jobQueue;
		std::mutex queueMutex;
		std::condition_variable condition;

		// Loop through all remaining tasks
		void run()
		{
			while (true)
			{
				Task task;
				{
					std::unique_lock<std::mutex> lock(queueMutex);
					condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
					if (destroying)
					{
						break;
					}
					task = jobQueue.front();
				}

				VOXEL_CORE_WARN("[THREAD POOL]: Thread worker {0} is running a task...", workerId)

				task();

				VOXEL_CORE_WARN("[THREAD POOL]: Thread worker {0} finished the task.", workerId)

				{
					std::lock_guard<std::mutex> lock(queueMutex);
					jobQueue.pop();
					condition.notify_one();
				}
			}
		}

	public:
		ThreadWorker(int workerId)
		{
			this->workerId = workerId;
			worker = std::thread(&ThreadWorker::run, this);

			VOXEL_CORE_WARN("[THREAD POOL]: Thread worker {0} created.", workerId)
		}

		~ThreadWorker()
		{
			if (worker.joinable())
			{
				wait();
				queueMutex.lock();
				destroying = true;
				condition.notify_one();
				queueMutex.unlock();
				worker.join();
			}

			VOXEL_CORE_WARN("[THREAD POOL]: Thread worker {0} terminated.", workerId)
		}

		// Add a new task to the thread's queue
		void addTask(Task function)
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			jobQueue.push(std::move(function));
			condition.notify_one();
		}

		// Wait until all work items have been finished
		void wait()
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [this]() { return jobQueue.empty(); });
		}
	};

	class ThreadPool
	{
	private:
		// Sets the number of threads to be allocated in this pool
		void setThreadCount(uint32 count)
		{
			threads.clear();
			for (auto i = 0; i < count; ++i)
			{
				threads.push_back(MakeUnique<ThreadWorker>(i));
			}
		}
	public:
		std::vector<UniqueRef<ThreadWorker>> threads;

		ThreadPool() noexcept = delete;
		ThreadPool(const uint32 workersCount)
		{
			setThreadCount(workersCount);
		}
		ThreadPool(const ThreadPool&) noexcept = delete;
		ThreadPool(ThreadPool&&) noexcept = delete;
		ThreadPool& operator=(const ThreadPool&) noexcept = delete;
		ThreadPool& operator=(ThreadPool&&) = delete;

		// Wait until all threads have finished their work items
		void waitAll()
		{
			for (auto& thread : threads)
			{
				thread->wait();
			}
		}
	};
}

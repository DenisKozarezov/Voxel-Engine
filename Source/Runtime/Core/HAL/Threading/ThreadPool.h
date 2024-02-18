#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <Core/CoreTypes.h>
#include <Core/Logging/Log.h>

namespace VoxelEngine::threading
{
	using Task = std::function<void()>;

	class ThreadWorker
	{
	private:
		bool m_destroying = false;
		int m_workerId;
		std::thread m_worker;
		std::queue<Task> m_jobQueue;
		std::mutex m_queueMutex;
		std::condition_variable m_condition;

		// Loop through all remaining tasks
		void run()
		{
			while (true)
			{
				Task task;
				{
					std::unique_lock<std::mutex> lock(m_queueMutex);
					m_condition.wait(lock, [this] { return !m_jobQueue.empty() || m_destroying; });
					if (m_destroying)
					{
						break;
					}
					task = m_jobQueue.front();
				}

				RUNTIME_WARN("[THREAD POOL]: Thread worker {0} is running a task...", m_workerId);

				task();

				RUNTIME_WARN("[THREAD POOL]: Thread worker {0} finished the task.", m_workerId);

				{
					std::lock_guard<std::mutex> lock(m_queueMutex);
					m_jobQueue.pop();
					m_condition.notify_one();
				}
			}
		}

	public:
		ThreadWorker(int workerId)
		{
			this->m_workerId = workerId;
			m_worker = std::thread(&ThreadWorker::run, this);

			RUNTIME_WARN("[THREAD POOL]: Thread worker {0} created.", workerId);
		}

		~ThreadWorker()
		{
			if (m_worker.joinable())
			{
				wait();
				m_queueMutex.lock();
				m_destroying = true;
				m_condition.notify_one();
				m_queueMutex.unlock();
				m_worker.join();
			}

			RUNTIME_WARN("[THREAD POOL]: Thread worker {0} terminated.", m_workerId);
		}

		// Add a new task to the thread's queue
		void addTask(Task function)
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_jobQueue.push(std::move(function));
			m_condition.notify_one();
		}

		// Wait until all work items have been finished
		void wait()
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_condition.wait(lock, [this]() { return m_jobQueue.empty(); });
		}
	};

	class ThreadPool
	{
	private:
		// Sets the number of threads to be allocated in this pool
		void setThreadCount(uint32 count)
		{
			threads.clear();
			for (uint32 i = 0; i < count; ++i)
			{
				threads.push_back(MakeUnique<ThreadWorker>(i));
			}
		}
	public:
		std::vector<TUniquePtr<ThreadWorker>> threads;

		ThreadPool() noexcept = delete;
		explicit  ThreadPool(const uint32 workersCount)
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

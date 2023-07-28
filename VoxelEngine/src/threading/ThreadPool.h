#pragma once
#include <queue>
#include <vector>
#include <future>
#include <core/Log.h>
#include "Task.h"

namespace VoxelEngine::threading
{
    class ThreadPool 
    {
    public:
        ThreadPool(const uint32 num_threads) 
        {
            threads.reserve(num_threads);
            for (int i = 0; i < num_threads; ++i) 
            {
                threads.emplace_back(std::thread([&, i]() { run(i); }));
            }

            VOXEL_CORE_TRACE("[Thread Pool] Reserving {0} thread workers.", num_threads)
        }

        template <typename Func, typename... Args>
        auto add_task(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>>
        {
            using returnType = std::invoke_result_t<Func, Args...>;
            auto task = std::make_shared<std::packaged_task<returnType()>>(
                std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

            const uint64_t task_id = last_idx++;

            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            tasks_info[task_id] = TaskInfo();
            lock.unlock();

            std::lock_guard<std::mutex> q_lock(q_mtx);
            q.emplace(Task(std::bind(std::forward<Func>(func), std::forward<Args>(args)...), std::forward<Args>(args)...), task_id);
            q_cv.notify_one();

            return task->get_future();
        }

        template <typename Func, typename ...Args, typename ...FuncTypes>
        uint64_t add_task(Func(*func)(FuncTypes...), Args&&... args) 
        {
            const uint64_t task_id = last_idx++;

            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            tasks_info[task_id] = TaskInfo();
            lock.unlock();

            std::lock_guard<std::mutex> q_lock(q_mtx);
            q.emplace(Task(func, std::forward<Args>(args)...), task_id);
            q_cv.notify_one();
            return task_id;
        }

        void wait(const uint64_t task_id) 
        {
            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx && tasks_info[task_id].status == TaskStatus::Completed;
                });
        }

        std::any wait_result(const uint64_t task_id)
        {
            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx && tasks_info[task_id].status == TaskStatus::Completed;
                });
            return tasks_info[task_id].result;
        }

        template<class T>
        void wait_result(const uint64_t task_id, T& value) 
        {
            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            tasks_info_cv.wait(lock, [this, task_id]()->bool {
                return task_id < last_idx && tasks_info[task_id].status == TaskStatus::Completed;
                });
            value = std::any_cast<T>(tasks_info[task_id].result);
        }

        void wait_all() 
        {
            std::unique_lock<std::mutex> lock(tasks_info_mtx);
            wait_all_cv.wait(lock, [this]()->bool { return cnt_completed_tasks == last_idx; });
        }

        bool calculated(const uint64_t task_id) 
        {
            std::lock_guard<std::mutex> lock(tasks_info_mtx);
            return task_id < last_idx && tasks_info[task_id].status == TaskStatus::Completed;
        }

        ~ThreadPool() 
        {
            VOXEL_CORE_TRACE("[Thread Pool] Joining all threading...")

            quite = true;
            q_cv.notify_all();
            for (int i = 0; i < threads.size(); ++i) {
                threads[i].join();
            }

            VOXEL_CORE_TRACE("[Thread Pool] All threads are joined.")
        }

    private:
        void run(int workerId) 
        {
            while (!quite) 
            {
                std::unique_lock<std::mutex> lock(q_mtx);
                q_cv.wait(lock, [this]()->bool { return !q.empty() || quite; });

                if (!q.empty() && !quite) 
                {
                    std::pair<Task, uint64_t> task = std::move(q.front());
                    q.pop();
                    lock.unlock();

                    VOXEL_CORE_TRACE("[Thread Pool] Worker {0}: Running task with id = {1}.", workerId, task.second)

                    task.first();

                    std::lock_guard<std::mutex> lock(tasks_info_mtx);
                    if (task.first.has_result()) {
                        tasks_info[task.second].result = task.first.get_result();
                    }

                    VOXEL_CORE_TRACE("[Thread Pool] Worker {0}. Task with id = {1} is completed.", workerId, task.second)

                    tasks_info[task.second].status = TaskStatus::Completed;
                    ++cnt_completed_tasks;
                }
                wait_all_cv.notify_all();
                tasks_info_cv.notify_all(); // notify for wait function
            }
        }

        std::vector<std::thread> threads;

        std::queue<std::pair<Task, uint64_t>> q;
        std::mutex q_mtx;
        std::condition_variable q_cv;

        std::unordered_map<uint64_t, TaskInfo> tasks_info;
        std::condition_variable tasks_info_cv;
        std::mutex tasks_info_mtx;

        std::condition_variable wait_all_cv;

        std::atomic<bool> quite{ false };
        std::atomic<uint64_t> last_idx{ 0 };
        std::atomic<uint64_t> cnt_completed_tasks{ 0 };
    };
}
#pragma once

namespace VoxelEngine::threading
{
	class ThreadPool final
	{
    public:
        explicit ThreadPool();

        ~ThreadPool();

        ThreadPool(ThreadPool const&) noexcept = delete;
        ThreadPool(ThreadPool&&) noexcept = delete;
        ThreadPool& operator = (ThreadPool const&) noexcept = delete;
        ThreadPool& operator = (ThreadPool&&) noexcept = delete;
	};
}
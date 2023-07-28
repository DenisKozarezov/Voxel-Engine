#pragma once
#include <core/PrimitiveTypes.h>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <any>
#include <type_traits>

namespace VoxelEngine::threading
{
	enum class TaskStatus : byte
	{
		Awaiting,
		Completed
	};

	struct TaskInfo 
	{
		TaskStatus status = TaskStatus::Awaiting;
		std::any result;
	};

	class Task
	{
	public:
		template <typename Func, typename ...Args>
		Task(const Func&& func, Args&&... args) :
			is_void{ std::is_void_v<Func> }
		{
			void_func = std::bind(func, args...);
			any_func = []()->int { return 0; };
		}

		template <typename FuncRetType, typename ...Args, typename ...FuncTypes>
		Task(FuncRetType(*func)(FuncTypes...), Args&&... args) :
			is_void{ std::is_void_v<FuncRetType> } 
		{
			if constexpr (std::is_void_v<FuncRetType>) 
			{
				void_func = std::bind(func, args...);
				any_func = []()->int { return 0; };
			}
			else {
				void_func = []()->void {};
				any_func = std::bind(func, args...);
			}
		}

		void operator() () 
		{
			void_func();
			any_func_result = any_func();
		}

		bool has_result() 
		{
			return !is_void;
		}

		std::any get_result() const 
		{
			return any_func_result;
		}

	private:
		std::function<void()> void_func;
		std::function<std::any()> any_func;
		std::any any_func_result;
		bool is_void;
	};
}
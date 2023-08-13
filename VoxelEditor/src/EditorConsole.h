#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	struct LogEntry
	{
	public:
		string level;
		string message;
		string time;

		LogEntry() noexcept = delete;

		template <typename... Args>
		LogEntry(const std::string_view& fmt, const Args&... args, const spdlog::level::level_enum& level = spdlog::level::info)
		{
			this->message = std::vformat(fmt, std::make_format_args(args...));
			this->level = VoxelEngine::logLevelToString(level);

			auto now = std::chrono::system_clock::now();
			this->time = std::vformat(FORMATTED_TIME, std::make_format_args(now));
		}
		template <typename... Args>
		LogEntry(const LogEntry& log) : 
			time(std::move(log.time)), 
			level(std::move(log.level)), 
			message(std::move(log.message))
		{ }

		~LogEntry() noexcept = default;

		inline const string str() const
		{ 
			return std::format("{0} [{1}] : {2}\n", time, level, message);
		}
	};

	class EditorConsole
	{
	private:
		ImGuiTextBuffer m_buffer;
		ImGuiTextFilter m_filter;
		ImVector<int> m_lineOffsets;
		bool m_consoleOpen = true;
		bool m_autoScroll;
		static EditorConsole* s_instance;

		void addLog(const char* fmt, ...);
	public:
		EditorConsole();

		template <typename... Args>
		inline static void info(std::string_view fmt, const Args&... args)
		{
			s_instance->addLog(LogEntry(fmt, args...).str().c_str());
		}
		template <typename... Args>
		inline static void warn(std::string_view fmt, const Args&... args)
		{
			s_instance->addLog(LogEntry(fmt, args..., spdlog::level::warn).str().c_str());
		}
		template <typename... Args>
		inline static void error(std::string_view fmt, const Args&... args)
		{
			s_instance->addLog(LogEntry(fmt, args..., spdlog::level::err).str().c_str());
		}
		void clear();
		void render();

		~EditorConsole();
	};
}
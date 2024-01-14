#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	struct LogEntry
	{
	public:
		string level;
		string message;
		string time;

		LogEntry() noexcept = delete;

		template <typename... Args>
		LogEntry(const spdlog::level::level_enum& level, const std::string_view& fmt, Args&&... args)
		{
			this->message = std::vformat(fmt, std::make_format_args(args...));
			this->level = logLevelString(level);

			auto now = std::chrono::system_clock::now();
			this->time = std::vformat(FORMATTED_TIME, std::make_format_args(now));
		}
		template <typename... Args>
		LogEntry(const LogEntry& log) : 
			time(log.time),
			level(log.level),
			message(log.message)
		{ }

		template <typename... Args>
		LogEntry(LogEntry&& log) : LogEntry(std::move(log.time), std::move(log.level), std::move(log.message))
		{ }

		~LogEntry() noexcept = default;

		INLINE const string str() const
		{ 
			return std::format("{0} [{1}] : {2}\n", time, level, message);
		}
	};

	class EditorConsole : public ImguiWindow
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
		EditorConsole(const string& title);

		template <typename... Args>
		INLINE static void info(std::string_view fmt, Args&&... args)
		{
			s_instance->addLog(LogEntry(spdlog::level::info, fmt, std::forward<Args>(args)...).str().c_str());
		}
		template <typename... Args>
		INLINE static void warn(std::string_view fmt, Args&&... args)
		{
			s_instance->addLog(LogEntry(spdlog::level::warn, fmt, std::forward<Args>(args)...).str().c_str());
		}
		template <typename... Args>
		INLINE static void error(std::string_view fmt, Args&&... args)
		{
			s_instance->addLog(LogEntry(spdlog::level::err, fmt, std::forward<Args>(args)...).str().c_str());
		}

		const ImGuiWindowFlags& flags() const override;

		void clear();
		void onImGuiRender() override;

		~EditorConsole() override;
	};
}
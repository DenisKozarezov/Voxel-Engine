#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
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
		static void log(std::string_view fmt, const Args&... args)
		{
			auto now = std::chrono::system_clock::now();
			std::string message = std::vformat(fmt, std::make_format_args(args...));
			std::string format = std::format("{0:%H:%M:%S} [{1}] : {2}\n", now, "info", message);
			s_instance->addLog(format.c_str());
		}
		template <typename... Args>
		static void warn(std::string_view fmt, const Args&... args)
		{
			auto now = std::chrono::system_clock::now();
			std::string message = std::vformat(fmt, std::make_format_args(args...));
			std::string format = std::format("{0:%H:%M:%S} [{1}] : {2}\n", now, "warn", message);
			s_instance->addLog(format.c_str());
		}
		template <typename... Args>
		static void error(std::string_view fmt, const Args&... args)
		{
			auto now = std::chrono::system_clock::now();
			std::string message = std::vformat(fmt, std::make_format_args(args...));
			std::string format = std::format("{0:%H:%M:%S} [{1}] : {2}\n", now, "error", message);
			s_instance->addLog(format.c_str());
		}
		void clear();
		void render();

		~EditorConsole();
	};
}
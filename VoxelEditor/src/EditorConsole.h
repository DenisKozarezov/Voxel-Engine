#pragma once
#include <imgui.h>

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
	public:
		EditorConsole();

		void addLog(const char* fmt, ...);
		void clear();
		void render();

		~EditorConsole();
	};
}
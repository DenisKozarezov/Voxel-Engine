#include "EditorConsole.h"

namespace VoxelEditor::gui
{
    EditorConsole* EditorConsole::s_instance = 0;

    EditorConsole::EditorConsole(const string& title) : ImguiWindow(title)
    {
        VOXEL_ASSERT(!s_instance, "There is other editor console instance!");

        s_instance = this;

        m_autoScroll = true;
        clear();
    }
    void EditorConsole::addLog(const char* fmt, ...)
    {
        int old_size = m_buffer.size();
        va_list args;
        va_start(args, fmt);
        m_buffer.appendfv(fmt, args);
        va_end(args);
        for (int new_size = m_buffer.size(); old_size < new_size; old_size++)
            if (m_buffer[old_size] == '\n')
                m_lineOffsets.push_back(old_size + 1);
    }
    const ImGuiWindowFlags& EditorConsole::flags() const
    {
        return ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    }
    void EditorConsole::clear()
    {
        m_buffer.clear();
        m_lineOffsets.clear();
        m_lineOffsets.push_back(0);
    }
    void EditorConsole::onImGuiRender()
	{
        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &m_autoScroll);
            ImGui::EndPopup();
        }

        // Main window
        bool isClear = ImGui::Button("Clear");
        ImGui::SameLine(0.0f, 2.0f);
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine(0.0f, 2.0f);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        m_filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (isClear)
            clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = m_buffer.begin();
        const char* buf_end = m_buffer.end();
        if (m_filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < m_lineOffsets.Size; line_no++)
            {
                const char* line_start = buf + m_lineOffsets[line_no];
                const char* line_end = (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
                if (m_filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(m_lineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + m_lineOffsets[line_no];
                    const char* line_end = (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
	}
    EditorConsole::~EditorConsole()
    {
        clear();
    }
}
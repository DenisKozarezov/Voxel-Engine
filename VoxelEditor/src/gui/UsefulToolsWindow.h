#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
    class UsefulToolsWindow : public ImguiWindow
    {
    public:
        UsefulToolsWindow(const string& title);

        void onImGuiRender() override;
    };
}
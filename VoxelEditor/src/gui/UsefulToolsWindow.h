#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
    class UsefulToolsWindow : public ImguiWindow
    {
    public:
        UsefulToolsWindow(const string& title);
        ~UsefulToolsWindow() override = default;
        
        void onImGuiRender() override;
    };
}
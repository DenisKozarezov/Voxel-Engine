#pragma once
#include "ImGuiWindow.h"

namespace VoxelEditor::gui
{
    class GuiTree;
    
    class UsefulToolsWindow : public ImguiWindow
    {
    private:
        GuiTree* m_guiTree = nullptr;
    public:
        UsefulToolsWindow(const string& title, GuiTree* guiTree);
        ~UsefulToolsWindow() override = default;
        
        void onImGuiRender() override;
    };
}
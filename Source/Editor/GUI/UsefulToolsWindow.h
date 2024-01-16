#pragma once
#include <Core/CoreMinimal.h>

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
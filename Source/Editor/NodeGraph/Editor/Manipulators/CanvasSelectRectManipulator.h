#pragma once
#include "ICanvasManipulator.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

namespace VoxelEditor::nodes
{
    class CanvasSelectRectManipulator : public ICanvasManipulator
    {
    private:
        bool m_mouseDown = false;
        ImVec2 m_startSelectPos{};
    public:
        CanvasSelectRectManipulator() = delete;
        CanvasSelectRectManipulator(const NodeEditor* nodeEditor);

        void manipulateGraph(NodeGraph& graph) override;
        
        ~CanvasSelectRectManipulator() override = default;
    };
}

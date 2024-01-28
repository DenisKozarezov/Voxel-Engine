#pragma once
#include "ICanvasManipulator.h"

namespace VoxelEditor::nodes
{
    class CanvasMoveManipulator : public ICanvasManipulator
    {
    public:
        CanvasMoveManipulator() = delete;
        CanvasMoveManipulator(const NodeEditor* nodeEditor);

        void manipulateGraph(NodeGraph& graph) override;
        
        ~CanvasMoveManipulator() override = default;
    };
}

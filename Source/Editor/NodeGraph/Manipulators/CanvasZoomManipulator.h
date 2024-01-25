#pragma once
#include "ICanvasManipulator.h"

namespace VoxelEditor::nodes
{
    class CanvasZoomManipulator : public ICanvasManipulator
    {
    public:
        CanvasZoomManipulator() = delete;
        CanvasZoomManipulator(const NodeEditor* nodeEditor);

        void manipulateGraph(NodeGraph& graph) override;
        
        ~CanvasZoomManipulator() override = default;
    };
}

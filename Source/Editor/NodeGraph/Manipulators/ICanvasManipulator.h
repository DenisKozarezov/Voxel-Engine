#pragma once
#include "../NodeEditor.h"

namespace VoxelEditor::nodes
{
    class ICanvasManipulator
    {
    protected:
        const NodeEditor* m_editor = nullptr;
    public:
        ICanvasManipulator() = default;
        ICanvasManipulator(const NodeEditor* nodeEditor);

        virtual void manipulateGraph(NodeGraph& graph) = 0;
        
        virtual ~ICanvasManipulator() = default;
    };
}

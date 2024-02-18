#include "ICanvasManipulator.h"
#include <NodeGraph/Editor/NodeEditor.h>

namespace VoxelEditor::nodes
{
    ICanvasManipulator::ICanvasManipulator(const NodeEditor* nodeEditor)
        : m_editor(nodeEditor)
    {

    }
}

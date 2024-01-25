#include "ICanvasManipulator.h"
#include "../NodeEditor.h"

namespace VoxelEditor::nodes
{
    ICanvasManipulator::ICanvasManipulator(const NodeEditor* nodeEditor)
        : m_editor(nodeEditor)
    {

    }
}

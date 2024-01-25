#include "ActionNode.h"

namespace VoxelEditor::nodes
{
    std::vector<TWeakPtr<NodeBase>> ActionNode::children() const
    {
        return std::vector<TWeakPtr<NodeBase>>(0);
    }
}

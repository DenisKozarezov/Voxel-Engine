#include "DecoratorNode.h"

namespace VoxelEditor::nodes
{
    std::vector<TWeakPtr<NodeBase>> DecoratorNode::children() const
    {
        return {m_child};
    }

    void DecoratorNode::addChild(const TWeakPtr<NodeBase>& child)
    {
        m_child = child;
    }

    void DecoratorNode::removeChild(const TWeakPtr<NodeBase>& child)
    {
        m_child.reset();
    }
}

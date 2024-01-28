#include "CompositeNode.h"

namespace VoxelEditor::nodes
{
    std::vector<TWeakPtr<NodeBase>> CompositeNode::children() const
    {
        return m_children;
    }

    void CompositeNode::addChild(const TWeakPtr<NodeBase>& child)
    {
        const auto childPtr = child.lock();
        if (!childPtr)
            return;
        
        const auto it = std::find_if(m_children.begin(), m_children.end(), [&](TWeakPtr<NodeBase>& el) -> bool
        {
            return !el.expired() && *el.lock() == *childPtr;
        });
        if (it == m_children.end())
        {
            m_children.emplace_back(child);
        }
    }

    void CompositeNode::removeChild(const TWeakPtr<NodeBase>& child)
    {
        const auto childPtr = child.lock();
        if (!childPtr)
            return;
        
        const auto it = std::find_if(m_children.begin(), m_children.end(), [&](TWeakPtr<NodeBase>& el) -> bool
        {
            return !el.expired() && *el.lock() == *childPtr;
        });
        if (it != m_children.end())
        {
            m_children.erase(it);
        }
    }
}

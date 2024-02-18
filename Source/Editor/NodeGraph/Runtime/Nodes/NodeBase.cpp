#include "NodeBase.h"

namespace VoxelEditor::nodes
{
    NodeBase::NodeBase(const string& name) : m_name(name)
    {
        
    }
    NodeBase::NodeBase(string&& name) : m_name(std::move(name))
    {
        
    }
    void NodeBase::setPosition(const ImVec2& position)
    {
        m_position = position;
    }
}

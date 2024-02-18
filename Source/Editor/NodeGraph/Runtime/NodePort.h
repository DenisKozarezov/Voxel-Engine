#pragma once
#include <Core/CoreDefines.h>

namespace VoxelEditor::nodes
{
    enum class PortDirection { Input, Output };
    
    class NodePort
    {
    private:
        PortDirection m_direction = PortDirection::Input;
    public:
        NodePort() = default;
        ~NodePort() = default;
        
        bool isConnected() const;
        bool IsCompatibleWith(const NodePort& port);
        FORCE_INLINE bool isInput() const { return m_direction == PortDirection::Input; }
        FORCE_INLINE bool isOutput() const  { return m_direction == PortDirection::Output; }
    };
}
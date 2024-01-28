#pragma once
#include <Core/CoreMinimal.h>
#include <imgui.h>

namespace VoxelEditor::nodes
{
    struct CanvasProperties;
    
    class NodeBase
    {
    private:
        string m_guid;
        string m_name;
        ImVec2 m_position{};
    public:
        NodeBase() = default;
        NodeBase(const string& name);
        NodeBase(string&& name);

        FORCE_INLINE const string& name() const { return m_name; }
        FORCE_INLINE const ImVec2& position() const { return m_position; }
        virtual std::vector<TWeakPtr<NodeBase>> children() const = 0;
        
        virtual void addChild(const TWeakPtr<NodeBase>& child) = 0;
        virtual void removeChild(const TWeakPtr<NodeBase>& child) = 0;
        void setPosition(const ImVec2& position);

        bool operator==(const NodeBase& rhs) const
        {
            if (this->m_name.empty() || rhs.m_name.empty())
                return false;            
            return this->m_name == rhs.m_name;
        }

        bool operator!=(const NodeBase& rhs) const
        {
            return !(*this == rhs);
        }
        
        
        virtual ~NodeBase() = default;
    };
}
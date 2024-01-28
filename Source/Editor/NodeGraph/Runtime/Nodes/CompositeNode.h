#pragma once
#include "NodeBase.h"

namespace VoxelEditor::nodes
{
    class CompositeNode : public NodeBase
    {
    private:
        std::vector<TWeakPtr<NodeBase>> m_children;
    public:
        CompositeNode(const string& name) : NodeBase(name) { }
        CompositeNode(string&& name) : NodeBase(name) { }

        std::vector<TWeakPtr<NodeBase>> children() const override;
        void addChild(const TWeakPtr<NodeBase>& child) final;
        void removeChild(const TWeakPtr<NodeBase>& child) final;
        
        ~CompositeNode() override = default;
    };
}

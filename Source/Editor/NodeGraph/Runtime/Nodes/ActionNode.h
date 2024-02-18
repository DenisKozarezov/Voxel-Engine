#pragma once
#include "NodeBase.h"

namespace VoxelEditor::nodes
{
    class ActionNode : public NodeBase
    {
    public:
        ActionNode(const string& name) : NodeBase(name) { }
        ActionNode(string&& name) : NodeBase(name) { }

        std::vector<TWeakPtr<NodeBase>> children() const override;
        void addChild(const TWeakPtr<NodeBase>& child) final { }
        void removeChild(const TWeakPtr<NodeBase>& child) final { }
        
        ~ActionNode() override = default;
    };
}

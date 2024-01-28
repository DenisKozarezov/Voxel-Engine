#pragma once
#include "NodeBase.h"

namespace VoxelEditor::nodes
{
    class DecoratorNode : public NodeBase
    {
    private:
        TWeakPtr<NodeBase> m_child;
    public:
        DecoratorNode(const string& name) : NodeBase(name) { }
        DecoratorNode(string&& name) : NodeBase(name) { }

        std::vector<TWeakPtr<NodeBase>> children() const override;
        void addChild(const TWeakPtr<NodeBase>& child) final;
        void removeChild(const TWeakPtr<NodeBase>& child) final;

        ~DecoratorNode() override = default;
    };
}

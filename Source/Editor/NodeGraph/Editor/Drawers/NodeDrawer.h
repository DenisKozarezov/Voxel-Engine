#pragma once
#include <Core/CoreDefines.h>

class ImDrawList;

namespace VoxelEditor::nodes
{
    struct CanvasProperties;
    class NodeBase;
    
    class NodeDrawer
    {
    private:
        const TSharedPtr<NodeBase>& m_node;
        const CanvasProperties* m_canvasProps;
    public:
        NodeDrawer() = delete;
        NodeDrawer(const TSharedPtr<NodeBase>& node, const CanvasProperties* canvasProps)
            : m_node(node), m_canvasProps(canvasProps) { }
        virtual ~NodeDrawer() = default;
        
        bool beginNode(ImDrawList* drawList);
        void endNode(ImDrawList* drawList);
    };
}
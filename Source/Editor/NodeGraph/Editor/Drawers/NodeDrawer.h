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
        const CanvasProperties* m_canvasProps;
    public:
        NodeDrawer() = delete;
        NodeDrawer(const CanvasProperties* canvasProps) : m_canvasProps(canvasProps) { }
        virtual ~NodeDrawer() = default;
        
        bool beginNode(ImDrawList* drawList, const TSharedPtr<NodeBase>& node);
        void endNode(ImDrawList* drawList, const TSharedPtr<NodeBase>& node);
    };
}
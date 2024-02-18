#include "NodeGraph.h"
#include "Nodes/DecoratorNode.h"
#include <NodeGraph/Editor/Drawers/NodeDrawer.h>

namespace VoxelEditor::nodes
{
    NodeGraph::NodeGraph()
    {
        auto root = MakeShared<DecoratorNode>("Root Node");
        root->setPosition({0, 0});
        addNode(root);
    }

    NodeGraph::~NodeGraph()
    {
    }
    
    void NodeGraph::renderGrid(ImDrawList* drawList, const float& gridSize)
    {
        const ImU32 gridColor = ImColor(m_canvasProps.colors[ColCanvasLines]);
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
        
        for (float x = fmodf(m_canvasProps.offset.x, gridSize); x < size.x;)
        {
            drawList->AddLine(ImVec2(x, 0.0f) + pos, ImVec2(x, size.y) + pos, gridColor);
            x += gridSize;
        }

        for (float y = fmodf(m_canvasProps.offset.y, gridSize); y < size.y;)
        {
            drawList->AddLine(ImVec2(0.0f, y) + pos, ImVec2(size.x, y) + pos, gridColor);
            y += gridSize;
        }
    }

    void NodeGraph::renderNodes(ImDrawList* drawList)
    {
        for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
        {
            auto node = *it;
            auto nodeDrawer = NodeDrawer(node, &m_canvasProps);
            
            if (nodeDrawer.beginNode(drawList))
            {
                auto* storage = ImGui::GetStateStorage();
                const float nodeWidth = storage->GetFloat(ImGui::GetID("node-width"));
                
                // Draw Node title
                const ImVec2 titleSize = ImGui::CalcTextSize(node->name().c_str());     
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + nodeWidth * 0.5f - titleSize.x * 0.5f);
                ImGui::TextUnformatted(node->name().c_str());

                storage->SetFloat(ImGui::GetID("node-width"), ImGui::GetItemRectSize().x);
                
                nodeDrawer.endNode(drawList);    
            }
        }
    }

    void NodeGraph::beginInputSlot(ImDrawList* drawList, const string& slotTitle)
    {        
        const float circleRadius = 5.f * m_canvasProps.zoom;
        ImVec2 titleSize = ImGui::CalcTextSize(slotTitle.c_str());
        ImColor color = m_canvasProps.colors[ColConnection];
        const auto& style = ImGui::GetStyle();
        float itemOffsetX = style.ItemSpacing.x * m_canvasProps.zoom;
        
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2{itemOffsetX, 0.0f});
        
        ImRect circleRect{
            ImGui::GetCursorScreenPos(),
            ImGui::GetCursorScreenPos() + ImVec2{circleRadius * 2.0f, circleRadius * 2.0f}
        };
        // Vertical-align circle in the middle of the line.
        float circleOffsetY = titleSize.y * 0.5f - circleRadius;
        circleRect.Min.y += circleOffsetY;
        circleRect.Max.y += circleOffsetY;
        drawList->AddCircleFilled(circleRect.GetCenter(), circleRadius, color);

        ImGui::ItemSize(circleRect.GetSize());
        ImGui::ItemAdd(circleRect, ImGui::GetID(slotTitle.c_str()));
    }

    void NodeGraph::beginOutputSlot(ImDrawList* drawList)
    {
    }

    void NodeGraph::endSlot()
    {
    }

    void NodeGraph::setZoom(const float& zoom)
    {
        m_canvasProps.zoom = zoom;
        ImGui::SetWindowFontScale(zoom);
    }

    void NodeGraph::addNode(const TSharedPtr<NodeBase>& newNode)
    {
        if (!m_rootNode)
            m_rootNode = newNode;

        const auto it = std::find(m_nodes.begin(), m_nodes.end(), newNode);
        if (it == m_nodes.end())
        {
            m_nodes.emplace_back(newNode);
        }
    }

    void NodeGraph::removeNode(const TSharedPtr<NodeBase>& node)
    {
        if (node == m_rootNode)
            m_rootNode = nullptr;
        
        const auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
        if (it != m_nodes.end())
        {
            m_nodes.erase(it, m_nodes.end());
        }
    }

    void NodeGraph::clear()
    {
        for (size_t i = 0; i < m_nodes.size(); ++i)
        {
            m_nodes[i].reset();
        }
        m_nodes.clear();
    }

    void NodeGraph::onImGuiRender(ImDrawList* drawList)
    {
        const float gridSize = 64.0f *  m_canvasProps.zoom;
        renderGrid(drawList, gridSize);

        renderNodes(drawList);

        beginInputSlot(drawList, "Some Variable");
    }
}

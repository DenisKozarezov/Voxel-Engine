#pragma once
#include <glm/vec3.hpp>
#include <imgui.h>

namespace utils
{
	static class ImGuiUtils
	{
	public:
		inline static void pushHeaderStyleColor(const ImColor& normalColor, const ImColor& hoveredColor, const ImColor& activeColor)
		{
			ImGui::PushStyleColor(ImGuiCol_Header, (ImVec4)ImColor::HSV(normalColor.Value.x, normalColor.Value.y, normalColor.Value.z));
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (ImVec4)ImColor::HSV(hoveredColor.Value.x, hoveredColor.Value.y, hoveredColor.Value.z));
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, (ImVec4)ImColor::HSV(activeColor.Value.x, activeColor.Value.y, activeColor.Value.z));
		}
	};
}

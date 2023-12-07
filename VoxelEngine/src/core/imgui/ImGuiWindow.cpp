#include "ImGuiWindow.h"

namespace VoxelEditor
{
	ImGuiWindow::ImGuiWindow(const string& title) : m_title(title)
	{

	}

	const bool ImGuiWindow::begin()
	{
		onBegin();

		bool keepVisible = true;
		ImGui::SetNextWindowSizeConstraints(
			ImVec2{ m_minSize[0], m_minSize[1] },
			ImVec2{ m_maxSize[0], m_maxSize[1] }
		);

		const bool notCollapsed = ImGui::Begin(m_title.c_str(), &keepVisible, flags());

		if (!keepVisible)
			hide();

		return notCollapsed;
	}
	void ImGuiWindow::end()
	{
		onEnd();

		m_hovered = ImGui::IsWindowHovered();
		ImGui::End();
	}
	void ImGuiWindow::setVisibility(const bool& isVisible)
	{
		if (m_visible == isVisible)
			return;

		m_visible = isVisible;
	}

	void ImGuiWindow::setMinSize(const float& minWidth, const float& minHeight)
	{
		m_minSize[0] = minWidth;
		m_minSize[1] = minHeight;
	}
	INLINE void ImGuiWindow::show()
	{
		setVisibility(true);
	}
	INLINE void ImGuiWindow::hide()
	{
		setVisibility(false);
	}
}
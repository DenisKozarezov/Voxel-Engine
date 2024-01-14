#include "ImGuiWindow.h"

namespace VoxelEngine
{
	ImguiWindow::ImguiWindow(const string& title) : m_title(title)
	{

	}

	const bool ImguiWindow::begin()
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
	void ImguiWindow::end()
	{
		onEnd();

		m_hovered = ImGui::IsWindowHovered();
		ImGui::End();
	}
	void ImguiWindow::setVisibility(const bool& isVisible)
	{
		if (m_visible == isVisible)
			return;

		m_visible = isVisible;
	}

	void ImguiWindow::setMinSize(const float& minWidth, const float& minHeight)
	{
		m_minSize[0] = minWidth;
		m_minSize[1] = minHeight;
	}
	INLINE void ImguiWindow::show()
	{
		setVisibility(true);
	}
	INLINE void ImguiWindow::hide()
	{
		setVisibility(false);
	}
}
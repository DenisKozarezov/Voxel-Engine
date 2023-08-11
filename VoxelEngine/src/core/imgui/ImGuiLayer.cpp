#include "ImGuiLayer.h"

namespace VoxelEngine::renderer
{
	void ImGuiLayer::onAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Color scheme
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		setStyle(ColorStyle::Dark);
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
	void ImGuiLayer::onDetach()
	{
		if (ImGui::GetCurrentContext())
			ImGui::DestroyContext();
	}
	void ImGuiLayer::onEvent(input::Event& e)
	{
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.isInCategory(input::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.isInCategory(input::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
	void ImGuiLayer::preRender()
	{
		ImGui::NewFrame();
	}
	void ImGuiLayer::postRender()
	{
		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
	void ImGuiLayer::setStyle(const ColorStyle& style) const
	{
		switch (style)
		{
		case Dark:
			ImGui::StyleColorsDark();
			break;
		case Light:
			ImGui::StyleColorsLight();
			break;
		case Classic:
			ImGui::StyleColorsClassic();
			break;
		}
	}
}
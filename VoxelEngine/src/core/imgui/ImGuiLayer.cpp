#include "ImGuiLayer.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace VoxelEngine::renderer
{
	void ImGuiLayer::onAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		setStyle(ColorStyle::Dark);
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
	void ImGuiLayer::onDetach()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::onEvent(input::Event& e)
	{
		if (_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.isInCategory(input::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.isInCategory(input::EventCategoryKeyboard) & io.WantCaptureKeyboard;
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
//#include "ImGuiLayer.h"
//#include <imgui/imgui.h>
//#include <imgui/backends/imgui_impl_glfw.h>
//#include <imgui/backends/imgui_impl_vulkan.h>
//#include "../Application.h"
//
//namespace VoxelEngine::renderer
//{
//	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") { }
//
//	void ImGuiLayer::onAttach()
//	{
//		// Setup context
//		IMGUI_CHECKVERSION();
//		ImGui::CreateContext();
//		ImGuiIO& io = ImGui::GetIO(); (void)io;
//		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
//		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
//		//io.ConfigViewportsNoAutoMerge = true;
//		//io.ConfigViewportsNoTaskBarIcon = true;
//
//		float fontSize = 18.0f;// *2.0f;
//		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
//		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);
//
//		ImGui::StyleColorsDark();
//		//ImGui::StyleColorsClassic();
//
//		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
//		ImGuiStyle& style = ImGui::GetStyle();
//		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//		{
//			style.WindowRounding = 0.0f;
//			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//		}
//
//		setDarkThemeColors();
//	}
//
//	void ImGuiLayer::onDetach()
//	{
//		ImGui_ImplVulkan_Shutdown();
//		ImGui_ImplGlfw_Shutdown();
//		ImGui::DestroyContext();
//	}
//
//	void ImGuiLayer::onEvent(input::Event& e)
//	{
//		if (_blockEvents)
//		{
//			ImGuiIO& io = ImGui::GetIO();
//			e.Handled |= e.isInCategory(input::EventCategoryMouse) & io.WantCaptureMouse;
//			e.Handled |= e.isInCategory(input::EventCategoryKeyboard) & io.WantCaptureKeyboard;
//		}
//	}
//
//	void ImGuiLayer::begin()
//	{
//		ImGui_ImplVulkan_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//		ImGui::NewFrame();
//	}
//
//	void ImGuiLayer::end()
//	{
//		//ImGuiIO& io = ImGui::GetIO();
//		//auto app = Application::getInstance();
//		//io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
//
//		//// Rendering
//		//ImGui::Render();
//		//ImDrawData* main_draw_data = ImGui::GetDrawData();
//		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData());
//
//		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
//		//{
//		//	GLFWwindow* backup_current_context = glfwGetCurrentContext();
//		//	ImGui::UpdatePlatformWindows();
//		//	ImGui::RenderPlatformWindowsDefault();
//		//	glfwMakeContextCurrent(backup_current_context);
//		//}
//	}
//
//	void ImGuiLayer::setDarkThemeColors()
//	{
//		auto& colors = ImGui::GetStyle().Colors;
//		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };
//
//		// Headers
//		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
//		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
//		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//
//		// Buttons
//		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
//		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
//		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//
//		// Frame BG
//		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
//		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
//		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//
//		// Tabs
//		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
//		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
//		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
//
//		// Title
//		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
//	}
//}
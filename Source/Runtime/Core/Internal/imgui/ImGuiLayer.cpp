#include "ImGuiLayer.h"
#include <Core/Logging/Log.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include "Core/HAL/AssetsManager/Paths.h"

namespace VoxelEngine::renderer
{
	void ImGuiLayer::loadLayout()
	{
		const string layoutPath = Paths::assetsDir() + "layouts/default.ini";
		
		try
		{
			if (layoutPath.empty())
				throw std::exception("Invalid path for ImGui layout!");

			if (!Paths::fileExists(layoutPath))
			{
				string error = "Unable to find an ImGui layout at path: " + layoutPath;
				throw std::exception(error.c_str());
			}
			
			RUNTIME_TRACE("Loading ImGui layout at path: {0}...", layoutPath);
			ImGui::LoadIniSettingsFromDisk(layoutPath.c_str());
		}
		catch (const std::exception& e)
		{
			RUNTIME_CRITICAL(e.what());
			RUNTIME_WARN("Loading ImGui default layout...");
			ImGui::LoadIniSettingsFromDisk(nullptr);
		}
	}

	void ImGuiLayer::saveLayout(const string& filepath)
	{
		ImGui::SaveIniSettingsToDisk(filepath.c_str());
	}

	void ImGuiLayer::onAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Color scheme
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
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

		loadLayout();
	}
	void ImGuiLayer::onDetach()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		if (ImGui::GetCurrentContext())
			ImGui::DestroyContext();
	}
	void ImGuiLayer::onEvent(input::Event& e)
	{
		if (m_blockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.hasCategoryFlag(input::EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.hasCategoryFlag(input::EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
	void ImGuiLayer::preRender()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
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

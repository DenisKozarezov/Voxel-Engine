#include "EditorLayer.h"
#include <imgui.h>

namespace VoxelEditor
{
	static bool show_demo_window = true;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{

	}

	const bool EditorLayer::onKeyPressed(const VoxelEngine::input::KeyPressedEvent& e) const
	{
		return false;
	}
	const bool EditorLayer::onMouseButtonPressed(const VoxelEngine::input::MouseButtonPressedEvent& e) const
	{
		return false;
	}

	void EditorLayer::onAttach()
	{				  
		
	}				  
	void EditorLayer::onDetach()
	{				  
	}				  
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{				  
	}				  
	void EditorLayer::onImGuiRender()
	{
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}				  
	void EditorLayer::onEvent(VoxelEngine::input::Event& e)
	{
	}
}
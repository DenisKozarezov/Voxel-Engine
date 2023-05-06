#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	class EditorApplication final : public VoxelEngine::Application
	{
	public:
		EditorApplication();
		EditorApplication(const EditorApplication&) = delete;
		~EditorApplication() = default;
	};
}

std::unique_ptr<VoxelEngine::Application> CreateApplication(int argc, char** argv)
{
	return std::make_unique<VoxelEditor::EditorApplication>();
}
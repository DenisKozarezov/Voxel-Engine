#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	class EditorApplication final : public VoxelEngine::Application
	{
	public:
		EditorApplication(const ApplicationSpecification& spec);
		EditorApplication(const EditorApplication&) = delete;
		~EditorApplication() = default;
	};
}

VoxelEngine::UniqueRef<VoxelEngine::Application> CreateApplication(ApplicationCommandLineArgs args)
{	
	ApplicationSpecification spec;
	spec.ApplicationName = "Voxel Editor";
	spec.Version = "v0.0.1";
	spec.WorkingDirectory = args[0];
	int index = spec.WorkingDirectory.find_last_of('\\');
	spec.WorkingDirectory = spec.WorkingDirectory.substr(0, index);
	spec.GraphicsAPI = "Vulkan";
	spec.CommandLineArgs = args;

	return VoxelEngine::MakeUnique<VoxelEditor::EditorApplication>(spec);
}
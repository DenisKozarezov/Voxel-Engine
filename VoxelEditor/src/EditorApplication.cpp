#include "EditorLayer.h"
#include <core/EntryPoint.h>

namespace VoxelEditor
{
	class EditorApplication final : public VoxelEngine::Application
	{
	public:
		EditorApplication(const ApplicationSpecification& spec) : Application(spec)
		{
			pushLayer(new EditorLayer());
		}
		EditorApplication(const EditorApplication&) noexcept = delete;
		EditorApplication(EditorApplication&&) noexcept = delete;
		~EditorApplication() noexcept = default;
	};
}

VoxelEngine::UniqueRef<VoxelEngine::Application> CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.ApplicationName = PROJECT_NAME;
	spec.Version = "v";
	spec.Version += PROJECT_VERSION;
	spec.WorkingDirectory = args[0];
	size_t index = spec.WorkingDirectory.find_last_of('\\');
	spec.WorkingDirectory = spec.WorkingDirectory.substr(0, index);
	spec.Maximized = true;
	spec.CommandLineArgs = args;

	return VoxelEngine::MakeUnique<VoxelEditor::EditorApplication>(spec);
}
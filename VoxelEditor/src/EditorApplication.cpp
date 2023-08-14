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
	spec.Version = string("v") + PROJECT_VERSION;
	spec.WorkingDirectory = g_currentPath;
	spec.Maximized = true;
	spec.CommandLineArgs = args;

	return VoxelEngine::MakeUnique<VoxelEditor::EditorApplication>(spec);
}
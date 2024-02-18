#include "EditorLayer.h"
#include <Core/Internal/EntryPoint.h>

namespace VoxelEditor
{
	class EditorApplication final : public Application
	{
	public:
		EditorApplication(const ApplicationSpecification& spec) : Application(spec)
		{
			pushLayer(new gui::EditorLayer());
		}
		EditorApplication(const EditorApplication&) noexcept = delete;
		EditorApplication(EditorApplication&&) noexcept = delete;
		~EditorApplication() override = default;
	};
}

TUniquePtr<Application> CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.ApplicationName = PROJECT_NAME;
	spec.Version = string("v") + PROJECT_VERSION;
	spec.WorkingDirectory = Paths::launchDir();
	spec.Maximized = true;
	spec.CommandLineArgs = args;

	return MakeUnique<VoxelEditor::EditorApplication>(spec);
}